/*  
    -- Autor: Stephen Luna Ramírez.
    -- Fecha: 17 de agosto a 09 de septiembre de 2025.
    -- Asignatura: Proyecto Terminal (Tesis).
    -- Instructor: Edwin Montes Orozco.
    ================================================================================================

    ### Implementación del metaheurístico Ant System.

    El algoritmo simula el comportamiento colectivo de hormigas artificiales para generar
    soluciones aproximadas a problemas de optimización bien definidos. En este trabajo se aplica
    al **análisis de robustez de redes complejas** (mediante métricas y restricciones), buscando
    la reducción/optimización del número de nodos conectados (o bien, maximizar el tamaño de dos
    componentes con un alto número mayor de grado).

    Las decisiones de construcción se basan en la combinación de rastro de feromonas e
    información heurística, permitiendo búsquedas rápidas de soluciones (exploración inicial,
    explotación posterior). **Ant System** es una variante dentro del paradigma ACO; para evitar
    convergencias prematuras puede incorporar mecanismos complementarios (i.e. técnicas tipo
    tabú) y depende del proceso de evaporación de feromonas para mantener diversidad en la búsqueda.

    Proceso esencial del algoritmo:

        1. Construcción de soluciones por las hormigas.
        2. Actualización del rastro de feromonas.
        3. Balance entre exploración y explotación (evaluación y refinamiento).
*/

// Conjunto de módulos desarrollados para adaptar y aplicar técnicas algorítmicas del
// Sistema de Hormigas. Incluyen implementaciones, herramientas de pre/postprocesado y
// utilidades de análisis para evaluar la robustez de redes complejas según métricas
// y restricciones del problema.

#include "AntSystem.h"  // Módulo de ejecución integral del Ant System.
                        // Contiene la puesta en marcha, el bucle iterativo, la construcción de soluciones,
                        // la actualización de feromonas y la evaluación de la función objetivo

#include "Feromona.h"   // La señal química colectiva (feromonas) constituye un componente fundamental del Ant System.
                        // El rastro depositado en las aristas codifica conocimiento heurístico que orienta la generación
                        // de soluciones, favorece la intensificación en regiones de alta calidad y permite exploraciones
                        // dirigidas hacia zonas prometedoras. Este mecanismo de realimentación es esencial para la
                        // convergencia eficiente hacia óptimos globales

#include "Globales.h"  // Contenedor de variables de alcance global.
                       // Almacena estructuras de datos y parámetros de control esenciales (p. ej. configuraciones,
                       // estados y parámetros del algoritmo) que son accedidos y actualizados a lo largo de todo el código

#include "Grafo.h"     /*
                            En el marco de la **Teoría de Grafos**, las redes funcionan como representaciones de adyacencia
                            que describen relaciones entre entidades en función de la similitud de sus características.
                            
                            Esta biblioteca está diseñada para la construcción y análisis de la topología de la red:
                            genera la representación de adyacencia, computa métricas topológicas relevantes (p. ej.
                            grado, centralidad, modularidad y tamaño de componentes) y permite la identificación y
                            extracción de subredes o componentes conexos —incluyendo, cuando aplica, hasta tres
                            subestructuras principales— necesarias para estudios rigurosos de robustez y vulnerabilidad.
                       */

// Módulos auxiliares implementados en C++ que proporcionan abstracciones 
// ligeras y utilidades de soporte para operaciones no esenciales, 
// facilitando la integración y el desarrollo.

#include <iostream>  // Biblioteca primordial para requisitos estándares de C++ (E/S)
#include <sstream>   // Biblioteca que permite convertir datos entre cadenas y otros tipos, favoreciendo el proceso de texto eficientemente
#include <cmath>     // Proporciona una amplia gama de funciones matemáticas para realizar cálculos complejos y comunes (exponenciales, logarítmicas, etc.)
#include <algorithm> // Ofrece una extensa funcionalidad para operaciones de búsqueda, ordenamiento, mezcla, funciones de conteo, entre otras
#include <random>    // Ofrece herramientas modernas y robustas para generar números aleatorios (en comparación de 'rand()', proporciona mejor calidad y mayor control)

// Inicializa las estructuras (datos fundamentales) de las hormigas
// para poder construir soluciones, actualizar el rastro de la 
// feromona y evaluar la F.O.
void inicializar_hormigas(vector<Hormiga> &hormigas, int tam_de_la_instancia) {
    hormigas.resize(m_hormigas); // Crea las estructuras para un determinado número de hormigas
    
    // Dado al número de hormigas
    for ( auto & ant : hormigas ) {
        // Asigna e inicializa los valores para cada estructuras de datos
        ant.lista_tabu.assign   (tam_de_la_instancia, false);
        ant.solucion.assign     (tam_de_la_instancia, false);
        ant.probabilidad.assign (tam_de_la_instancia, 0.0);
        ant.funcion_objetivo =  {0.0};
    }

} // Fin de la función inicializar_hormigas

/*
    Arranque del metaheurístico Ant System.
    
    Permite determinar qué nodos de la red serán desconectados, produciendo una partición
    en tres componentes principales: 

        - un componente separador, 
        - y dos subredes (A y B) con alto número de nodos (grado/tamaño).
    
    La solución hallada por cada hormiga asigna un valor booleano a los nodos:
        
        false (0) -> nodo conectado,
        true  (1) -> nodo desconectado.

    Esta información se emplea para calcular el coste de la función objetivo
    en el análisis de robustez de la red.
*/

void ejecutar_ant_system (vector<Hormiga> &hormigas, short tipo_de_grafo) {
    // Número de elementos que cada hormiga evaluará, visitará/marcará en su lista tabú
    const int dimension_del_problema      = hormigas[0].lista_tabu.size(); 
    // Cantidad total de entidades activas (hormigas artificiales en busca de buenas soluciones)
    const int numero_de_hormigas          = hormigas.size();
    // Declara la acumulada que poseera valores probabilisticos de selección (inicializado en cero)
    vector<double> acumulada(dimension_del_problema, 0.0); 
    // Declara dos estructuras vacías para optimizar el cálculo de potencia en la probabilidad
    vector<double> feromona_alpha;
    vector<double> grado_beta;      
    /*
        Generador aleatorio de alta calidad y alto rendimiento. Se emplea una instancia por hebra mediante la especificación thread_local, 
        la cual proporciona almacenamiento independiente por hilo: cada hilo dispone de su propia copia del generador con duración de 
        almacenamiento por hilo. Esta estrategia elimina la necesidad de sincronización global sobre la fuente de aleatoriedad, evitando 
        bloqueos y contención y, por tanto, mejorando el rendimiento en entornos multihilo.

        No obstante, conviene precisar que el presente proyecto no está concebido para implementar la metaheurística mediante paralelismo de 
        hilos o núcleos; la paralelización no constituye una vía garantizada para mejorar la calidad de la solución y, además, exige rediseños 
        (control de semillas por hilo para reproducibilidad, gestión de estructuras compartidas, verificación de condiciones de carrera, etc.). 
        
        En consecuencia, 'thread_local' se justifica aquí como medida para evitar costes de sincronización en caso de uso concurrente puntual, 
        pero cualquier decisión de paralelizar debe basarse en un rediseño deliberado y en perfiles empíricos que valoren el **costo-beneficio** entre 
        velocidad, reproducibilidad y complejidad de implementación.
    */         
    static thread_local mt19937 motor_estocastico(random_device{}()); 
    uniform_real_distribution<double> distribucion_real   (0.0, 1.0); // Define el rango de valores probabilísticos en [0.0, 1.0], no limitado estrictamente a 0 y 1
    uniform_int_distribution<int>     distribucion_binario(0, 1);     // Define el rango del conjunto solución de la hormiga: {0, 1} (conexión / desconexión)
    
    // Optimiza los exponentes matemáticos para la heurística (una sola vez por cada actualización de feromonas)
    preparar_componentes_de_potencia(feromona_alpha, grado_beta);

    // Operación controlada en función de la cantidad de hormigas definidas en el sistema
    for ( int ant = 0; ant < numero_de_hormigas; ++ant ) {
        // Para reducir la sobrecarga asociada a accesos repetidos por cada hormiga, 
        // se utiliza una referencia/local alias a la estructura u objeto correspondiente
        auto &agente_heuristico = hormigas[ant];
        // Representa aquellos nodos generados que no son válidos 
        // (número de probabilidad aleatoria no correcta)
        int  errores_en_el_indice_tabu = dimension_del_problema;
        // Contador de nodos marcados como visitados en la lista tabú.
        // Indica qué nodos no deben ser reconsiderados durante la construcción de la solución.
        // Esta técnica permite comprobaciones en O(1) si el contador se mantiene actualizado
        int  nodos_tabu = 0;
        
        // Mientras la hormiga no posee lista tabú completa (aún no ha encontrado una solución)
        while ( !lista_tabu_completa(agente_heuristico.lista_tabu, nodos_tabu) ) {
            /*----------------------------------------------------------------------------------------------------------------------------------
                Arranca la simulación del sistema de hormigas junto con el componente heurístico,
                ejecutando la exploración de rutas y la construcción de soluciones óptimas            
            */
            obtener_probabilidad_de_transición_al_nodo_i(agente_heuristico, acumulada, feromona_alpha, grado_beta);  // Aplica la fórmula de Probabilidad de Selección
            double selector_estocastico = distribucion_real(motor_estocastico);                        // Genera un valor aleatorio [0.0, 1.0] para seleccionar un nodo según la distribución acumulada
            int    nodo_elegido         = seleccionar_nodo_por_rango(acumulada, selector_estocastico); // Selecciona el nodo que será desconectado de la red de manera heurística

            // Nodo inválido: el selector estocástico no alcanzó ningún rango definido
            if ( nodo_elegido == -1 ) nodo_elegido = --errores_en_el_indice_tabu; // En cada iteración, reduce en '1' los índices de la lista tabú, forzando la elección de un nodo (de manera decremental)
       
            // Comprueba que la posición evaluada no se encuentre registrada 
            // en la lista tabú de nodos previamente visitados
            if ( !agente_heuristico.lista_tabu[nodo_elegido] ) {
                agente_heuristico.solucion[nodo_elegido]   = distribucion_binario(motor_estocastico); // Añade el nodo elegido a la solución y genera un valor aleatorio entre 0 (conectado) y 1 (desconectado)
                agente_heuristico.lista_tabu[nodo_elegido] = true;                                    // Marca el nodo como visitado en la lista tabú y confirma la solución encontrada
                
                ++nodos_tabu; // Incrementa a uno el número de nodos marcados como visitados
            }

        } // Fin del bucle while: concluye la construcción de la solución de la hormiga

        // Verifica si será posible escribir todo
        // en el archivo para el resultado de análisis
        if ( imprimir_resultado ) escribir_solucion_en_el_archivo(agente_heuristico.solucion, ant);

        // Evalúa y determina el costo asociado a la función objetivo generado por una hormiga específica
        funcion_objetivo(agente_heuristico, ant, tipo_de_grafo);

    } // Fin del bucle for: recorrido final de las hormigas
    /*------------------------------------------------------------------------------
        Una vez que las hormigas han finalizado sus recorridos, se ejecuta la
        actualización del rastro de feromonas: (1) aplicación de la evaporación
        (factor **Rho**) para atenuar rastros previos, y (2) depósito de feromona en
        las aristas/vértices recorridos en función de la calidad de las soluciones.

        Esta secuencia preserva la diversidad de búsqueda y evita la convergencia
        prematura hacia óptimos locales.
    */
    evaporar_feromona_global(hormigas);

    // Verifica si es posible mostrar en salida los resultados completos del análisis de robustez (Ant System)
    if ( imprimir_resultado ) {
        string mensaje = "\nModelo de Evaporación de Feromonas como Regulador de Convergencia en Ant System\n\n"
                         "Tras la fase de exploración, las trayectorias más prometedoras quedan resaltadas por la deposición de\n"
                         "feromona; dichas trazas refuerzan rutas que han conducido a soluciones con menor coste de la función\n"
                         "objetivo. La fase de evaporación actúa como mecanismo regulador: atenúa de forma continua la intensidad\n"
                         "de los rastros, evitando la acumulación excesiva de feromona sobre soluciones tempranas y, con ello, la\n"
                         "convergencia prematura hacia óptimos subóptimos.\n\n"
                         
                         "En términos de **diseño algorítmico**, la tasa de evaporación es un parámetro de control crítico que equilibra\n"
                         "explotación y exploración: una evaporación insuficiente favorece la explotación (posible estancamiento),\n"
                         "mientras que una evaporación excesiva puede degradar la información útil y forzar exploración innecesaria.\n"
                         "Por tanto, la política de actualización debe combinar depósitos proporcionales a la calidad de las soluciones\n"
                         "encontradas con una evaporación calibrada —y, preferiblemente, validada mediante perfilado empírico—\n"
                         "para garantizar robustez y eficacia en la búsqueda de soluciones óptimas.\n\n";

        imprimir_vector_de_feromonas(mensaje, nombre_de_archivo_Feromona);
    }

} // Fin de la función ejecutar_ant_system

/*
    Regla probabilística que decide el siguiente nodo a visitar por una hormiga en su recorrido,
    orientando la búsqueda hacia el conjunto separador. La fórmula de selección (probabilidad)
    y los fundamentos teóricos se describen extensamente en la memoria/tesis, que incluye
    definiciones sobre redes complejas, el problema combinatorio considerado y la variante
    Ant System implementada.

    **Nota**: Ver sección correspondiente de la tesis para la expresión formal de la probabilidad.
*/

void obtener_probabilidad_de_transición_al_nodo_i(Hormiga &ant, vector<double> &acumulada, const vector<double> &feromona_alpha, const vector<double> &grado_beta) {
    /*    
        Tras la construcción parcial de una solución por una hormiga, la elección del próximo
        vértice se realiza mediante una regla probabilística que considera dos factores principales:

            1. Cantidad de feromonas: grado de atracción hacia un nodo, que crece con las visitas
               acumuladas de las hormigas sobre la arista correspondiente.

            2. Visibilidad: heurística local (p. ej. inverso del coste o proximidad) que mide la
               idoneidad inmediata del candidato.

        Ambos factores se combinan y ponderan (por ejemplo, mediante parámetros Alpha y Beta) para
        equilibrar exploración y explotación: favorecer rutas prometedoras sin descartar
        alternativas potencialmente mejores.
    */
    
    // Inicializa en cero el total de la probabilidad acumulativa
    double acumulado = 0.0;

    // Suma de valores de transición (feromona × heurística) sobre vecinos válidos, excluyendo la lista tabú; 
    // sirve como denominador en la probabilidad de selección.
    const double suma_de_transición_valida   = calcular_el_denominador_de_seleccion(ant.lista_tabu, feromona_alpha, grado_beta);
    // Cardinalidad o número de probabilidades a evaluar
    const int    tam_probabilidad_transicion = ant.probabilidad.size();
    
    // Operación dependiente del tamaño de la instancia, es decir, del número de elementos
    // considerados en la distribución de probabilidad P(i).
    for ( int prob = 0; prob < tam_probabilidad_transicion; ++ prob ) {
        // Inicializa en cero la probabilidad de selección para 
        // reiniciar cálculos tras elegir nodos en la solución
        double p = 0.0; 
        
        // Verifica si la lista tabú no contiene ningún nodo seleccionado.
        if ( !ant.lista_tabu[prob] ) {
            /*
                Evalúa la probabilidad de transición hacia el nodo (i). 
                Ésta se obtiene mediante:

                    [ P(i) = ( T(i)^BETA * V(i)^ALPHA ) / Σ[T(u)^BETA * V(u)^ALPHA] ]

                Donde:
                
                    - T(i): Cantidad de feromona en el nodo (i).
                    - V(i): Valor heurístico o visibilidad asociada al nodo (i).
                    - ALPHA, BETA: Parámetros de control que ponderan la importancia
                      relativa de heurística y feromona en el proceso de decisión.
            */
            p = ( grado_beta[prob] * feromona_alpha[prob] ) / suma_de_transición_valida;
        }

        // Registra el valor calculado de probabilidad en el conjunto de probabilidades
        // asociado a los nodos para su posterior uso en la selección
        ant.probabilidad[prob] = p; 
        // Obtiene la suma acumulada de todas las probabilidades de transición,
        // utilizada para determinar si es necesario normalizar y garantizar
        // una distribución acumulada válida
        acumulado += p;
        // Registra el valor actual de la probabilidad en el acumulador,
        // sumándolo al valor previamente acumulado para mantener la lógica acumulativa
        acumulada[prob] = acumulado; // i.e. (valor_previo + valor_actual)

    } // Fin bucle for

    // Verifica si la suma acumulada no alcanza el valor esperado (1.0)
    if ( necesita_normalizar(acumulado) ) {
        // Normaliza cada valor dividiéndolo entre la suma total acumulada
        for ( double &a : acumulada ) {
            a /= acumulado;
        }
    }

} // Fin de la función obtener_probabilidad_de_transición_al_nodo_i

// Normaliza las probabilidades de transición entre nodos candidatos (excluyendo los nodos
// presentes en la lista tabú). La normalización integra la heurística y la intensidad de
// feromonas para producir probabilidades relativas utilizadas en la selección.
double calcular_el_denominador_de_seleccion(const vector<bool> &lista_tabu, const vector<double> &feromona_alpha, const vector<double> &grado_beta) {
    double    denominador_de_seleccion {0.0};      // Inicializa en cero la sumatoria de valores de transición (heurística × feromona)
    const int numero_de_nodos = lista_tabu.size(); // Número total de nodos que requiere visitar (elegir) la lista tabú
    
    // Dado al número de nodos que posee la red (tamaño de la instancia)
    for ( int nodo = 0; nodo < numero_de_nodos; ++nodo ) {
        // Si en la lista tabú no posee nodos elegidos
        if ( !lista_tabu[nodo] ) {
            // Evalúa el denominador de la fórmula de probabilidad de selección, 
            // considerando todos los rastros de feromona relevantes.
            denominador_de_seleccion += grado_beta[nodo] * feromona_alpha[nodo];
            /*
                NOTA: Dado que Alpha y Beta no varían entre llamadas, se reutiliza el precálculo
                      correspondiente, incorporando únicamente las actualizaciones de feromonas. 
                      La selección de nodos válidos para la probabilidad se controla mediante la lista tabú
            */
        }
    }
    // Posteriormente
    return denominador_de_seleccion; // Regresa el resultado final 

} // Fin de la función calcular_el_denominador_de_seleccion

// Determina si la distribución acumulada necesita ser normalizada, 
// garantizando que el valor final alcance 1.0 y que los valores 
// intermedios permanezcan crecientes y ordenados.
inline bool necesita_normalizar(double total) {
    // Verifica si la distribución acumulada requiere 
    // normalización para que el valor final sea 1.0.
    return ( total > 0.0 && ( fabs(total - 1.0) > 1e-6) );
} // Fin de la función necesita_normalizar

/*
    Dado que el cálculo del grado de cada nodo —y, análogamente, la evaluación de la contribución de cada rastro de feromona— 
    requiere elevar valores a la potencia del parámetro de control Beta, el uso intensivo de 'std::pow' resulta costoso desde el 
    punto de vista computacional. Por ello, es aconsejable optimizar estas operaciones validando previamente si los parámetros 
    de control (Alpha y Beta) adoptan valores enteros pequeños o constantes conocidas; en dichos casos pueden sustituirse las llamadas 
    a potencias generales por multiplicaciones sucesivas, desplazamientos bit a bit o tablas de consulta precomputadas, reduciendo 
    la carga aritmética y los tiempos de ejecución.

    Adicionalmente, las llamadas repetidas a funciones implican la creación de marcos de pila que penalizan el rendimiento. 
    En este contexto, la especificación **inline** puede emplearse para sugerir al compilador la expansión de la rutina en el 
    punto de invocación, eliminando la sobrecarga asociada a la llamada y mejorando la latencia. No obstante, la expansión 
    indiscriminada puede aumentar el tamaño del binario (código) y afectar negativamente a la localización en caché; por tanto, su 
    aplicación debe ser selectiva y respaldada por perfiles de rendimiento.

    En una palabra, se recomienda priorizar optimizaciones algorítmicas (simplificación de potencias y otros) y aplicar 
    transformaciones a nivel de compilador únicamente tras análisis empíricos de rendimiento, con el fin de maximizar 
    la eficiencia temporal sin comprometer la complejidad espacial ni la mantenibilidad del código del Ant System para el 
    análisis de robustez en redes complejas.
*/

inline double refinar_componentes_de_potencia(double base, double exponente) {
    constexpr double limite_de_aceptacion = 1e-9; // Umbral de tolerancia para comparar valores en punto flotante

    // Si el valor del exponente representa a cero (base^0)
    if ( fabs(exponente)       < limite_de_aceptacion ) return 1.0;  // El valor siempre será 1.0
    // De lo contrario, si el exponente es uno (base^1)
    if ( fabs(exponente - 1.0) < limite_de_aceptacion ) return base; // Siempre obtendrá el mismo número
    // De otra manera
    double entero_redondeado = round(exponente); // Redondea el exponente con parte fraccionaria 

    // Comprueba que el valor entero sea positivo, relativamente pequeño y que se encuentre
    // dentro del rango definido por el algoritmo evolutivo (Evolución Diferencial).
    // En este caso, los parámetros de control Alpha y Beta deben estar en el intervalo [0.1, 5.0]
    if ( fabs(exponente - entero_redondeado) < limite_de_aceptacion ) {
        if ( entero_redondeado > 0 && entero_redondeado <= 5 ) {
            // Potencia del valor (grado o feromona), inicializada en 1
            double potencia = 1.0; 

            // Determina, en función del exponente, el número de iteraciones 
            // de multiplicación aplicadas al valor base
            for ( int p = 0; p < static_cast<int> (entero_redondeado); ++p ) {
                potencia *= base; 
            }
            // Finalmente
            return potencia; // Devuelve el resultado final: la potencia calculada de la base
        }
    }
    // De lo contrario

    // Si el exponente representa la raíz cuadrada (base^1/2)
    if ( fabs(exponente - 0.5)          < limite_de_aceptacion ) return sqrt(base); 
    // De otra manera, puede representar raíz cúbica (base^1/3)
    if ( fabs(exponente - (1.0 / 3.0) ) < limite_de_aceptacion ) return cbrt(base);

    // Si ninguna de las opciones fue correcta
    return pow(base, exponente); // Utiliza el caso general
    
} // Fin de la función refinar_componentes_de_potencia

// Optimiza el cálculo de exponentes en el modelo heurístico de probabilidad de selección
void preparar_componentes_de_potencia(vector<double> &feromona_alpha, vector<double> &grado_beta) {
    const int nivel_de_feromonas = feromona.size(); // La intensidad total de feromonas en una arista/nodo
    feromona_alpha.resize(nivel_de_feromonas);      // Cantidad de feromona para un determinado nodo 
    grado_beta.resize    (nivel_de_feromonas);      // Asigna el número de nodos para el valor heurístico

    // Dado al conjunto total de feromonas
    for ( int nivel = 0; nivel < nivel_de_feromonas; ++nivel ) {
        // Realiza el precálculo de la probabilidad de transición asociada a un nodo determinado,
        // considerando la influencia de feromonas y heurística
        feromona_alpha[nivel] = refinar_componentes_de_potencia(feromona[nivel], ALPHA);
        grado_beta[nivel]     = refinar_componentes_de_potencia(nodos[nivel].grado, BETA);
    }
} // Fin de la función preparar_componentes_de_potencia


// A partir del número aleatorio probabilístico, se determina el rango acumulado
// al que pertenece, identificando así el nodo que será desconectado de la red
// como parte de la solución construida por el algoritmo
int seleccionar_nodo_por_rango(const vector<double> &acumulada, double umbral_de_seleccion) { 
    /*
    int       posicion_de_la_solucion {-1};                  // Representa el identificador único asociado a la solución construida por una hormiga determinada
    const int tam_del_conjunto_seleccion = acumulada.size(); // Cantidad de elementos que posee la distribución acumulativa

    // Dado al conjunto de probabilidades acumulativas
    for ( int indice_prob = 0; indice_prob < tam_del_conjunto_seleccion; ++indice_prob ) {
        // Verifica que el número aleatorio (valor heurístico) esté definido en un rango específico
        if ( umbral_de_seleccion <= acumulada[indice_prob] ) {
            posicion_de_la_solucion = indice_prob; // En caso afirmativo, registra el identificador o índice del nodo que representa la solución
            break;                                 // Una vez localizada la solución, se detiene el proceso para prevenir iteraciones innecesarias
        }
    }
    // Finalmente
    return posicion_de_la_solucion; // Proporciona el índice o posición del nodo correspondiente a la separación identificada en la red
    */

    /*-------------------------------------------------------------------------------------------------------------------------------------------------------------
        
        Rememorando conceptos de **Análisis y Diseño de Algoritmos**, una *búsqueda lineal* presenta, en el peor de los casos, complejidad temporal 
        T(n) = O(n), mientras que la *búsqueda binaria* ofrece T(n) = O(log(n)) en el peor caso (la base del logaritmo es 2 en la formulación clásica, 
        aunque la notación asintótica es independiente de la base). Es importante subrayar que la búsqueda binaria requiere que los datos estén ordenados 
        y que el acceso aleatorio sea eficiente; en ausencia de estas condiciones, su uso no es aplicable.

        En términos prácticos, para valores pequeños de (n), las constantes y la sobrecarga de implementación pueden dominar el comportamiento observado,
        de modo que un algoritmo con peor orden asintótico puede resultar igual o más rápido en la práctica. Por ejemplo, si T(n) = O(n), entonces, para
        [ n = 8 ] requieren aproximadamente **8 operaciones** en el peor caso y para [ n = 1000 ] unas 1000; en cambio, para O(log(n)) y [ n = 1000 ] se 
        requieren alrededor de log_2(1000) ~= 10 comparaciones, lo que ilustra la ventaja asintótica para tamaños grandes.
        
        No obstante, los resultados empíricos—por muy exhaustivos que sean—no reemplazan el análisis teórico: deben complementarse. Las pruebas experimentales 
        aportan información sobre constantes ocultas, localización en caché, predicción de saltos y patrones de datos reales, mientras que el análisis asintótico 
        guía la elección para escalabilidad. Por tanto, una estrategia práctica consistirá en implementar ambas alternativas y seleccionar dinámicamente la más 
        apropiada en función de la configuración (p. ej. si la colección está ordenada) y del tamaño de la red; el umbral de conmutación debe determinarse mediante 
        perfiles de rendimiento.

        A continuación, presento lo siguiente:

    ---------------------------------------------------------------------------------------------------------------------------------------------------------------*/

    /*
        Cantidad de operaciones T(n).

        Representa el número total de operaciones ejecutadas, 
        clasificadas en **simples** y **complejas**, utilizadas 
        para el análisis del algoritmo
    */
    const size_t n = acumulada.size();             
    // Define el límite superior permitido para el parámetro (n) en la función T(n) 
    // de complejidad temporal. Aquí, (n) representa el número de nodos de la red
    constexpr size_t ENTRADA_MAXIMA_TN = 25; // **Nota**: Si el usuario lo desea, puede cambiar el valor, PERO **menor** a 100
    
    // Si dentro a la llamada de la función
    // no posee ningún dato (casi imposible que suceda)
    if ( n == 0 ) return -1; // Retorna el dato o índice inválido para la lista tabú (selección de nodos)

    // Si (n) para T(n) no es grande (mayor peso)
    if ( n <= ENTRADA_MAXIMA_TN ) {
        // Realiza Búsqueda Lineal (común para cualquier programa sencillo)
        for ( size_t i_prob = 0; i_prob < n; ++i_prob ) {
            // Verificando que esté dentro del 
            // rango definido al valor heurístico
            if ( umbral_de_seleccion <= acumulada[i_prob] ) {
                return ( static_cast<int> (i_prob) );
            }
        } // De otra manera 
        return -1; // No fue válida
    }
    else { // De lo contrario, (n) es altísimo 
           // Por tanto: Realiza Búsqueda Binaria (más eficiente y tiempos de ejecución reducido)

        // Aunque sería factible implementar manualmente el procedimiento para reducir 
        // referencias locales al objeto, ello implicaría mayor tiempo y extensión de código. 
        // En su lugar, la función 'std::lower_bound' de la biblioteca <algorithm> ofrece 
        // una implementación optimizada para dicho propósito
        auto iterador = lower_bound(acumulada.begin(), acumulada.end(), umbral_de_seleccion); 
        
       // Verifica que el iterador no apunte al final 
       // (es decir, que el nodo fue encontrado)
                                                // Si el nodo es encontrado, se calcula la distancia entre el inicio del 
                                                // vector y el iterador correspondiente, resultando en un entero que 
                                                // representa la posición del elemento dentro de la estructura
        if ( iterador != acumulada.end() ) return ( static_cast<int> (distance(acumulada.begin(), iterador)) ); 
        else                               return -1; // De otra manera, no existe un nodo válido
    }

} // Fin de la función seleccionar_nodo_por_rango

/*
    **Política tabú para diversificación de la búsqueda**

    Técnica heurística destinada a prevenir la vuelta sobre soluciones o configuraciones
    ya exploradas y consideradas no deseables. 
    
    La lista tabú memoriza movimientos/estados visitados para restringir temporalmente su 
    selección, promoviendo así la exploración de nuevas regiones del espacio de búsqueda y 
    mejorando la eficiencia del proceso metaheurístico (Ant System) en la obtención de 
    soluciones de calidad.
*/

/* -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool lista_tabu_completa(const vector<bool> &lista_tabu) { // RECORDATORIO: Analizarlo a detalle para saber si puede o no ser O(n) (i.e. si es necesario recorrer todo el vector o arreglo)
    // Opera sobre un conjunto definido de nodos de la red
    for ( const auto &tabu : lista_tabu ) {
        // Verifica si no hay nodos seleccionados
        if ( !tabu ) return false;
    }
    // De otra manera
    return true; // Todos los nodos fueron visitados (i.e. la lista tabú es ya completa)

} // Fin de la función lista_tabu_completa
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

/*
    Tras un análisis repetido de la función encargada del mecanismo tabú —responsable de seleccionar nodos, 
    marcarlos como visitados y excluirlos temporalmente para explorar nuevas regiones del espacio de 
    soluciones— se ha demostrado que el coste del proceso de selección puede reducirse de manera significativa 
    mediante diseño algorítmico. En particular, manteniendo estructuras incrementales (por ejemplo: índices directos) 
    y actualizaciones locales en lugar de recorridos completos, la operación de selección/exclusión puede ejecutarse en 
    tiempo constante amortizado O(1) por decisión, frente al coste O(n) que supone un barrido lineal del arreglo.

*/

inline bool lista_tabu_completa(const vector<bool> &lista_tabu, int nodos_tabu_visitados) {
    // Por lo tanto: en lugar de recorrer toda la lista tabú para localizar nodos no visitados, 
    // se utiliza un contador de nodos marcados (true). Si el contador alcanza el tamaño de la lista,
    // la lista tabú está completa; en caso contrario, aún existen nodos por visitar.
    // Esta estrategia evita escaneos O(n) innecesarios y simplifica la comprobación
    return ( nodos_tabu_visitados == static_cast<int> (lista_tabu.size()) );
} // Fin de la función lista_tabu_completa

// Permite escribir en un archivo de texto la solución generada
// por una hormiga específica, facilitando la visualización y análisis
// de los resultados obtenidos por el Ant System
void escribir_solucion_en_el_archivo(const vector<bool> &ant_solution, int id) {
    // Comprueba si es posible escribir la solución 
    // de la hormiga en el archivo
    if ( !solucion_final ) {
        cout << "**IMPORTANTE**: Se ha detectado un posible error durante la creación o apertura del archivo destinado a\n"
             << "\trepresentar correctamente la solución generada por una hormiga específica. Este archivo indica qué nodos\n"
             << "\tserán completamente aislados de la red (marcados con un valor de '1'). Se recomienda verificar la ubicación\n"
             << "\tdel archivo en el directorio correspondiente y reintentar la operación.\n\n";

        exit(1); // Advierte y finaliza la ejecución del programa sin realizar cambios oportunos
    }

    int separador_de_secuencia = 0;

    ostringstream oss;
    oss << "El agente (hormiga artificial) <<" << (id + 1) << ">> "
            "ha localizado una solución viable en el espacio de búsqueda\n\n";
    oss << "[ ";
    
    for (  bool solucion : ant_solution ) {
        oss << solucion << ' ';
        if ( ++separador_de_secuencia % 35 == 0 ) oss << "\n  ";
    }
    oss.put(']');
    oss.put('\n');
    oss.put('\n');

    solucion_final << oss.str();
} // Fin de la función escribir_solucion_en_el_archivo

// A partir de las soluciones generadas por las hormigas se identifica cómo fragmentar la red:
// qué nodos deben separarse del componente gigante para formar dos subcomponentes de gran tamaño.
// Con la partición resultante se calcula el coste de la **función objetivo**, que constituye la base
// del **análisis de robustez** y orienta la búsqueda hacia la optimización (maximización o minimización)
void funcion_objetivo(Hormiga &ant, int hormiga_id, short modelo_de_adyacencia) {
   // Verifica si es posible crear y escribir los archivos según el número de hormigas 
   // y soluciones en las redes fragmentadas (visualización gráfica)
    if ( imprimir_resultado ) {
        // Define el nombre del archivo de salida que almacenará la fragmentación de la red (máx. 55 caracteres)
        char nombre_del_archivo[450]; 

        snprintf(nombre_del_archivo, sizeof(nombre_del_archivo), "Resultados_Empiricos_Optimizacion/"
                "Desconexion_Estructural_Causada_por_AntSystem/descomposicion_de_red_HORMIGA_%03d.txt", hormiga_id);

        // Verifica el modelo de representación de la red para ejecutar la partición simulada
        if ( modelo_de_adyacencia == 0 ) red_fragmentada_MATRIZ(ant, nombre_del_archivo);
        else                             red_fragmentada_LISTA (ant, nombre_del_archivo);
    }
    /*
        Declara los **tres componentes principales** para el análisis de robustez:

        (1) Componente separador (nodos desconectados).
        (2) Dos componentes mayores (A y B) resultantes de la partición.
    */
    vector<int> componentes_de_robustez = encontrar_componentes_conectados(ant, hormiga_id, modelo_de_adyacencia);
    /*
       A partir de este punto, se establecen los tamaños de los tres componentes principales,
       asignando la cantidad de nodos correspondiente a S (separador), A y B (ambos con mayor número de grado)
    */
    int tam_del_componente_S = componentes_de_robustez[S]; 
    int tam_del_componente_A = componentes_de_robustez[A];
    int tam_del_componente_B = componentes_de_robustez[B];
    /*
        Para determinar la función objetivo, se verifica si los nodos pueden ser alcanzados
        bajo un porcentaje válido que actúa como restricción. En función de ello,
        el costo será **maximizado** o **minimizado** según el criterio definido.
    */
    const int numero_total_de_nodos    = ant.solucion.size();
    // Obtiene el número total de nodos de la red, omitiendo aquellos que forman parte
    // del conjunto separador, ya que éstos no deben considerarse en el conteo
    double    numero_de_nodos_validos  = numero_total_de_nodos - tam_del_componente_S;
    // Determina el porcentaje de cobertura de nodos considerando dos conjuntos específicos,
    // en relación con la cantidad total de nodos disponibles en la red
    double    restriccion_de_costo     = ( static_cast<double> ( tam_del_componente_A + tam_del_componente_B ) / numero_de_nodos_validos ) * 100;

    /*
        Dadas las cardinalidades de los componentes A y B, se verifican las condiciones
        predefinidas y se determina cuál función objetivo (coste) corresponde aplicar.
        
        Esta elección guía la evaluación final de la partición y la comparación entre soluciones.
    */
    
    // Define la cobertura mínima exigida (70%) que se usa como criterio para
    // evaluar el coste de la función objetivo en el análisis de robustez
    constexpr float COBERTURA_DE_PARTICION_REQUERIDA = 70.0; 

    // Si alguno de los dos componentes mayores (A o B) queda vacío, aplica una penalización: [ coste += N_total + |S| ]
    if ( tam_del_componente_B == 0 ) tam_del_componente_B = numero_total_de_nodos + tam_del_componente_S;
    if ( tam_del_componente_A == 0 ) tam_del_componente_A = numero_total_de_nodos + tam_del_componente_S;
    
    // Comprueba si la suma de las cardinalidades de A y B (|A| + |B|) 
    // es mayor o igual al 70% del número total de nodos de la red
    if ( restriccion_de_costo >= COBERTURA_DE_PARTICION_REQUERIDA ) {
        ant.funcion_objetivo = tam_del_componente_S; // Minimiza la función objetivo (mejor costo alcanzado)
    }
    else { // Si no se cumple la restricción, se incrementa la cardinalidad del componente B 
           // con la suma de todos los nodos de la red más el tamaño del separador (|S|)
        tam_del_componente_B = numero_total_de_nodos + tam_del_componente_S;
        ant.funcion_objetivo = tam_del_componente_B; // Por lo tanto: Maximiza la función objetivo (peor costo)
    }

    antSystem << '\n' << ">> Valor de la Función Objetivo (Costo estimado): " << ant.funcion_objetivo;
    antSystem.put('\n');
    antSystem.put('\n');

    /*
        Para cada hormiga se documenta el valor de la función objetivo. Paralelamente,
        se mantienen los estadísticos relevantes —en particular el coste mínimo (mejor)
        y el coste máximo (peor)— que serán reportados en la sección de resultados
        para el análisis comparativo y la evaluación del desempeño del algoritmo.
    */

    if ( ant.funcion_objetivo < costo_optimo_encontrado ) costo_optimo_encontrado = ant.funcion_objetivo;
    if ( ant.funcion_objetivo > costo_maximo_observado  ) costo_maximo_observado  = ant.funcion_objetivo;

    antSystem << "-- Registro acumulado de los valores de la Función Objetivo hasta la iteración presente --";
    antSystem.put('\n');
    antSystem.put('\n');
    antSystem << "\t>> Valor Óptimo (criterio de minimización en la F.O.):  " << costo_optimo_encontrado << '\n';
    antSystem << "\t>> Valor Máximo (criterio de maximización en la F.O.):  " << costo_maximo_observado  << '\n';

} // Fin de la función funcion_objetivo

