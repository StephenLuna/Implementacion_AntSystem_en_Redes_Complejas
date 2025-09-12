
/*
    -- Autor: Stephen Luna Ramírez.
    -- Fecha: 17 de agosto a 09 de septiembre de 2025.
    -- Asignatura: Proyecto Terminal (Tesis).
    -- Instructor: Edwin Montes Orozco.
    =========================================================================================

    Algoritmo heurístico Ant System: técnica probabilística fundamentada en la **feromona**.
    La distribución de feromonas guía el comportamiento colectivo y, por ende, las soluciones
    generadas, permitiendo la exploración/explotación de rutas próximas al óptimo.

    En este trabajo se persigue la minimización de ciertos nodos (aislamiento de la red),
    y cada solución determina un coste para la función objetivo que se modifica
    (mejora o empeora) en función de las restricciones impuestas.
*/

// Librerías personalizadas que implementan los mecanismos fundamentales del Sistema de Hormigas.
// Incluyen rutinas encargadas de la inicialización y configuración de la FEROMONA.

#include "Feromona.h"  // Las hormigas (y, en general, el algoritmo) necesitan el rastro de la feromona para buscar caminos óptimos (búsqueda de buenas soluciones)
#include "AntSystem.h" // Metaheurística necesaria para resolver el problema de optimización (buscar la minimización de nodos en la red)
#include "Globales.h"  // Uso de variables globales (Estructuras de datos, valores booleanos, entre otros)

// Bibliotecas de apoyo que ofrecen herramientas secundarias y funciones de conveniencia en C++, 
// diseñadas para simplificar tareas rutinarias

#include <iostream>   // Biblioteca NECESARIA para la utilidad de funciones estándares de C++ (Input/Output)
#include <sstream>    // Permite trabajar con cadenas de texto de manera similar a archivos o entrada estándar, facilitando la E/S
#include <cmath>      // Bibloteca que proporciona cualquier uso matemático (valor absoluto, potencia, etc.)

// Crea e inicializa la distribución (colección) de feromona para todas las hormigas
// de tal manera que al ser llamada una hormiga, puede ocupar ésta como guía de comportamiento
// (permitiendo, a su vez, encontrar la solución óptima posible)
void establecer_linea_de_base_de_feromonas(int intensidad_de_la_ruta) {
    // Reserva la cantidad de feromona que será depositada 
    // o estará presente en una trayectoria específica 
    feromona.resize(intensidad_de_la_ruta, 1.0);       // Los niveles de la feromona estarán inicializados en 1's
} // Fin de la función establecer_linea_de_base_de_feromona

// Muestra el estado actual de la feromona utilizado 
// por un número determinado de hormigas
void imprimir_vector_de_feromonas(string mensaje, const char *nombre_del_archivo) {
    // Contador que separa la distribución de feromona por un determinado número de secuencia
    int imprimir_lote_de_feromona = 0; 
    ofstream estado_de_feromonas(nombre_del_archivo);
    
    // Proporciona un manejo eficiente y de alto rendimiento para cadenas de texto,
    // optimizando las operaciones de construcción y manipulación
    ostringstream oss;
    oss << mensaje << "| "; 

    for ( const auto &f : feromona ) {
        oss << f << ' ';
        // Si la distribución de feromona alcanzó a 30 niveles
        if ( ++imprimir_lote_de_feromona % 30 == 0 ) oss << "\n  "; // Ofrece formato y separa para mejor visualización
    }
    oss.put('|');
    oss.put('\n');
    oss.put('\n');
    // Con ello, escribe todos los resultado en un único paso
    estado_de_feromonas << oss.str();
} // Fin de la función imprimir_vector_de_feromonas

// La feromona se evapora gradualmente (coeficiente Rho), reduciendo su intensidad y la atracción
// hacia rutas antiguas. Esto hace que los caminos cortos, recorridos con mayor frecuencia,
// mantengan una densidad relativa de feromonas superior frente a rutas largas.
// La **evaporación** es esencial para prevenir la acumulación permanente de feromonas y el
// estancamiento de la búsqueda, preservando el equilibrio entre exploración y explotación.
void evaporar_feromona_global(const vector<Hormiga> &hormigas) {
    /*
        Cálculo del estado de evaporación (Rho).

        Dado que el optimizador (Evolución Diferencial) puede generar valores fuera del rango [0,1]
        (o negativos), se debe normalizar 'Rho' usando su valor absoluto y aplicar un recorte/normalización
        al intervalo válido. Valores negativos o fuera de rango provocan actualizaciones incorrectas
        del rastro de feromonas y pueden dar lugar a **ciclos infinitos** en la lista tabú.
    */
    const double factor = abs(1.0 - RHO);

    // Considera simultáneamente la fuerza de atracción y la intensidad asociada a cada ruta,
    // como factores determinantes en el proceso de selección
    for ( auto &rastro : feromona ) {
        rastro *= factor; // Inicia el proceso de evaporación de feromonas: (1 - RHO) × Feromona(t-1)
    }
    /*-------------------------------------------------------------------------------------
        Se actualiza el rastro sumando una cantidad proporcional al inverso del coste
        obtenido por la hormiga. 
        
        Los nodos **no incluidos** en el Componente Separador reciben una penalización 
        (reducción o exclusión de la aportación) para evitar reforzar soluciones triviales 
        o no útiles. Esta estrategia prioriza la calidad de las soluciones y fomenta la 
        exploración de nuevas regiones del espacio de búsqueda.
    */
    actualizar_feromona_inversa(hormigas);

} // Fin de la función evaporar_feromona_global

/*
    La feromona constituye el rastro de memoria colectiva que orienta la selección
    probabilística de sucesores durante la construcción de soluciones. En esta implementación
    se emplea un "estado inverso" de la feromona (equivalente a invertir el criterio de la
    función objetivo) para priorizar rutas asociadas a costes bajos —es decir, favorecer la
    **minimización del número de nodos en la red**—.
 
    Para prevenir la sobreexplotación y la convergencia prematura hacia soluciones subóptimas
    se combinan:

        - Evaporación del rastro (actualización multiplicativa con (1 - Rho)), y

        - Omisión/penalización de nodos no pertenecientes al Componente Separador (nodos desconectados),
        que evita reforzar rutas no útiles.
 
    El efecto combinado preserva diversidad en la búsqueda (exploración) y permite la
    intensificación sobre soluciones de calidad, mejorando la probabilidad de hallar mínimos relevantes.
*/

void actualizar_feromona_inversa(const vector<Hormiga> &hormigas) {
    // Representa el tamaño total del conjunto de feromonas: número de entradas del vector
    // que almacenan las intensidades de feromona entre nodos/aristas
    const int dimension_de_feromonas = feromona.size();

    // Define la cantidad de hormigas artificiales utilizadas como agentes exploradores,
    // cuya función es guiar la búsqueda hacia caminos óptimos dentro de la red
    for ( const auto &ant : hormigas ) {
        // Verifica si el costo de la función 
        // objetivo es cero (solución exacta u óptima)
        if ( ant.funcion_objetivo == 0 ) continue; // Excluye información no relevante con el fin de controlar la presión selectiva
                                                   // y mantener la coherencia en la escala de actualización
        
        // Calcula el inverso de la función objetivo para la solución de una hormiga
        const double funcion_objetivo_inverso = 1.0 / ant.funcion_objetivo;
        // Utiliza una referencia local para minimizar el 'overhead' de acceso
        // en cada iteración, mejorando así la eficiencia computacional
        const auto &solucion = ant.solucion; 
        
        // Según el número total de nodos de la red
        for ( int nodo = 0; nodo < dimension_de_feromonas; ++nodo ) {
            // Integra el valor inverso de la función objetivo a la feromona 
            // asociada a la hormiga, considerando exclusivamente los nodos 
            // que han sido separados de la red
            if ( solucion[nodo] ) feromona[nodo] += funcion_objetivo_inverso;
        }
    }

} // Fin de la función actualizar_feromona_inversa

/*
    Inicia la ejecución del algoritmo heurístico (denominado, Sistema de Hormigas)

    Durante cada iteración se generan soluciones (basado al número de hormigas),
    se aplican actualizaciones de feromonas (inicio de estado de evaporación) 
    y se evalúan restricciones y costes asociados a la función objetivo
*/

void aplicar_rastro_de_feromona(int tam_de_la_instancia, short tipo_grafo) {
    // Inicializa el comportamiento **global** y **colectivo** de las hormigas
    establecer_linea_de_base_de_feromonas(tam_de_la_instancia); 

    // Si es posible, imprime todo en el archivo de salida
    if ( imprimir_resultado ) {
        string mensaje = "\nRastro Inicial de Feromonas\n\n"
                         "Todas las hormigas aún no han comenzado la búsqueda. En breve, al menos una iniciará su recorrido\n"
                         "y orientará a sus compañeras hacia rutas más cortas y eficientes a través de la red, contribuyendo\n"
                         "a la obtención de soluciones que minimicen, en la medida de lo posible, el número de nodos conectados.\n\n";

        const char nombre_del_archivo[] = "Resultados_Empiricos_Optimizacion/rastro_GLOBAL_de_feromonas.txt";

        imprimir_vector_de_feromonas(mensaje, nombre_del_archivo);
    } 

    antSystem << "\n\n:: Parametros Equilibrados Requeridos ::\n"
                  << "-----------------------------------------------------\n"
                  << "<< Número de Hormigas [por crear]: "    << m_hormigas
                  << "\n<< Número de Iteraciones (t-1):    "  << num_iteraciones
                  << "\n<< Alpha = " << ALPHA
                  << "\n<< Beta  = " << BETA
                  << "\n<< Rho   = " << RHO << "\n"
                  << "-----------------------------------------------------\n\n\n";

    antSystem << "-->> Inicia la Ejecución del Sistema de Hormigas (Ant System) <<--\n\n";

    /*
            Registro iterativo por hormiga.

            Mantiene una lista de eventos (datos) visitados por cada hormiga,
            que se actualiza en cada iteración (t). Por ejemplo: para construir
            soluciones, calcular costes, gestionar la lista tabú, entre otras.
    */

    vector<Hormiga> agentes_de_soluciones;
    // Define el nombre del archivo, restringido a un máximo de 54 caracteres
    char archivo_de_solucion[300];      

    // Operación controlada por el parámetro de iteración (t) (ciclo actual del algoritmo)
    for ( int t = 0; t < num_iteraciones; ++t ) {

        /*
           Por razones de legibilidad y buenas prácticas no es recomendable mezclar constructos 
           específicos de C con las características de C++; en este trabajo se ha elegido C++ como 
           lenguaje principal para la implementación del Ant System aplicado al análisis de robustez 
           en redes complejas. 
           
           No obstante, cuando se prioriza el rendimiento en operaciones concretas de formateo de 
           salida —por ejemplo, para crear los ficheros que contienen las soluciones generadas por 
           cada hormiga en una iteración (t) se ha optado por utilizar la función C 'snprintf()', 
           dada su eficiencia y su control explícito sobre los búferes.
        */

        if ( imprimir_resultado ) {
            // Genera el nombre de archivo según la iteración de ejecución (soluciones de las hormigas)
            snprintf(archivo_de_solucion, sizeof(archivo_de_solucion), "Resultados_Empiricos_Optimizacion/"
                    "Soluciones_Emergentes_Redes_AntSystem/soluciones_por_hormiga_t%03d.txt", t);
            // Posteriormente, se abre el archivo a fin de escribir las soluciones correspondientes a cada hormiga
            solucion_final.open(archivo_de_solucion);

            // Construye el nombre del archivo asociado al registro de actualizaciones de feromonas,
            // indexado por el número de iteración (t), para su posterior almacenamiento y análisis
            snprintf(nombre_de_archivo_Feromona, sizeof(nombre_de_archivo_Feromona), "Resultados_Empiricos_Optimizacion/"
                    "Estado_Temporal_Feromonas/iteracion_%03d_del_registro_de_feromonas.txt", t);
        }

        antSystem << '\n' << "Distribución de feromonas — iteración <" << t << "> (concentración por arista/nodo)" << '\n';
        antSystem << "\n========================================================================================================" << '\n';

        // Reutiliza el vector de agentes artificiales (hormigas), evitando
        // destrucción/recreación, y asegurando el uso eficiente de recursos
        agentes_de_soluciones.clear();
        // Asigna el espacio requerido en memoria para la creación de las hormigas
        // y ejecuta su inicialización con los parámetros definidos por el algoritmo
        inicializar_hormigas(agentes_de_soluciones, tam_de_la_instancia); 

        /*
            Inicia la ejecución del **Ant System**: los agentes-hormiga construyen soluciones que definen
            qué nodos permanecen conectados o desconectados, generando componentes/subredes.

            Con la partición resultante se calcula el coste de la función objetivo y, cuando procede,
            se evalúa su estado complementario/invertido para el análisis.
        */

        ejecutar_ant_system (agentes_de_soluciones, tipo_grafo);

        if ( imprimir_resultado ) solucion_final.close(); // Cierra el archivo para prevenir errores de escritura
        
        // Si el algoritmo evolutivo no es activado
        if ( !es_algoritmoED_activado ) { 
            // Advierte que todas las soluciones serán creadas (escritas) en un archivo específico
            antSystem << "\nLa solución resultante ha sido generada y exportada al archivo [ " << archivo_de_solucion << " ],\n"
                         "donde se documentan los detalles pertinentes.\n"; 
        }
    }

    if ( !es_algoritmoED_activado ) {
        antSystem << "-----------------------------------------------------------------------------------------------------"
                  << "---------------------------------------------------------------------";
        antSystem.put('\n');
        antSystem.put('\n');
        antSystem.put('\n');

        antSystem << "**Notificación de Relevancia Crítica**" << '\n';

        antSystem << "\nA partir de las soluciones generadas por cada hormiga —cada una determina el coste de la función objetivo\n"
                     "— se obtiene una partición de la red que, en el escenario considerado, produce tres componentes\n"
                     "principales y separa el resto del grafo (componente gigante). Sobre esa partición se efectúa el análisis de\n"
                     "robustez mediante métricas de redes complejas (grado, centralidad, coeficiente de clustering) para construir\n"
                     "la función de coste cuyo objetivo es minimizar la cardinalidad del separador.\n\n"
                         
                     "El fichero [ descomposicion_de_red_HORMIGA_00x.txt ] registra la fragmentación asociada a la solución\n"
                     "óptima (ruta y exploración de la hormiga): un valor '0' indica **conectividad** entre nodos, '1' indica **separación\n"
                     "total**, y se listan explícitamente las aristas [(u, v)] y los nodos resultantes para facilitar la interpretación y la\n"
                     "validación experimental.\n\n";
    }
    /*
        Cierra el archivo de salida para garantizar la correcta finalización
        del proceso de escritura y prevenir pérdida de datos.
    */
    antSystem.close();
} // Fin de la función aplicar_rastro_de_feromona

