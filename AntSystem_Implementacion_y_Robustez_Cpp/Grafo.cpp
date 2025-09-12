
/*
    -- Autor: Stephen Luna Ramírez.
    -- Fecha: 17 de agosto a 09 de septiembre de 2025.
    -- Asignatura: Proyecto Terminal (Tesis).
    -- Instructor: Edwin Montes Orozco.
    =================================================================================================================

    El módulo **Grafo** constituye el punto de partida de la implementación: proporciona la representación
    computacional del grafo, la carga y validación de la instancia (fichero de entrada con nodos y aristas)
    y las operaciones básicas necesarias para ejecutar el metaheurístico y efectuar el **análisis de robustez**. Está
    diseñado para integrarse con las rutinas de optimización y las métricas de redes complejas que sustentan el
    estudio.
*/


/*
    Bibliotecas propiamente diseñadas para un objetivo específico 
    (comportamiento y mecanismos particulares para la manipulación 
    y representación de la entrada a partir de una estructura de 
    datos lineal: Matriz o Lista de Adyacencia). 
*/

#include "Grafo.h"  // Interpreta el sistema mediante un grafo 
                    // y a través de técnicas como la Percolación Inversa y VSP, 
                    // utiliza métricas como el grado y la centralidad para desconectar 
                    // la red de manera inteligente

#include "Instancia_del_Problema.h" // Para este programa sólo se utiliza la funcionalidad de borrar cualquier dato impreso desde la terminal                      
#include "Globales.h"               // Cualquier valor global que será utilizado en el programa (Estructuras de Datos, variables, etc.) 

/*
    Bibliotecas que poseen reglas exclusivas (funciones estándares) para
    el lenguaje de programación C++
*/

#include <iostream>  // Biblioteca Estándar (Input/Output)

#include <iterator>  // Herramienta fundamental para el uso explícito de iteradores, 
                     // los cuales permiten recorrer y manipular elementos en contenedores (vector, list, deque, etc.)

#include <numeric>   // Biblioteca que proporciona una colección de algoritmos numéricos que pueden ser aplicados a 
                     // secuencias de datos (i.e. realizar operaciones de acumulación, producto, etc.)

#include <algorithm> // Biblioteca que proporciona una colección de funciones para realizar operaciones sobre 
                     // contenedores y rangos de datos (incluye búsqueda, ordenamiento, conteo, etc.)
                     // En este caso, se utiliza la función 'any_of' para determinar si un nodo (nodo (u))                
                     // es conectado en la red (en representación matricial)

#include <deque>    // Proporciona un contenedor denominado *double-ended queue* (cola de doble extremo), el cual permite insertar y eliminar elementos de 
                    // manera eficiente tanto al principio como al final (i.e. puede actuar como las siguientes estructuras de datos lineales: pila, lista, cola, etc.)

#include <sstream>  // Biblioteca que proporciona clases para manipular cadenas de texto como si fueran flujos de E/S (i.e cout y cin)


// Genera en **texto plano** la estructura de la red a través de una Matriz de Adyacencia
// Constituida por ceros (nodos desconectados) y unos (nodos conectados)
void generar_Matriz_Adyacencia(ofstream &red) {
    // Si el programa no fue llevado a cabo mediante 
    // el algoritmo evolutivo (calibración de parámetros)
    if ( !es_algoritmoED_activado ) {
        short seleccion_de_impreso;  // La opción de impreso, introducido por el usuario
        tipo_de_modo_de_impresion(); // El menú que muestra las diferentes opciones que puede seleccionar el usuario mismo
        
        // Mientras la selección no sea válida
        while ( true ) {
            printf("\nPor favor, registre a continuación la modalidad de impresión seleccionada (1 o 2): "); 
            cin >> seleccion_de_impreso; // Lee la opción del usuario

            if ( seleccion_de_impreso == 1 || seleccion_de_impreso == 2 ) { // Si alguna de las opciones es válida
                printf("\n\tEjecución en fase de inicio. Espere mientras se completa el proceso...\n");
                this_thread::sleep_for(chrono::seconds(2));
                limpiar_pantalla();
            }
            else {
                printf("\n\nLa opción introducida no corresponde a ninguna de las alternativas válidas. Por\n"
                           "favor, ingrese únicamente las opciones previamente indicadas\n\n");
                continue;                           
            }

            // Verifica qué tipo de impreso se desea ejecutar
            if ( seleccion_de_impreso == 1 ) {
                crear_matrizAdyacencia_para_todos_los_nodos(red);
                break;               
            }
            else
                if ( seleccion_de_impreso == 2 ) {
                    generar_matriz_de_nodos_conectados(red);
                    break;
                }
        }
    }
    else { // De lo contrario
        generar_matriz_de_nodos_conectados(red); // Muestra, de manera más eficiente y rápida, la Matriz de Adyacencia (la red)
    }
} // Fin de la función generar_Matriz_Adyacencia

// Simple función que muestra las opciones posibles que el usuario 
// puede usar para imprimir una red como Matriz de Adyacencia
void tipo_de_modo_de_impresion() {

    printf("\n\tSELECCIÓN DEL MÉTODO DE IMPRESIÓN DE UNA RED EN MATRIZ DE ADYACENCIA\n\n"
               
            "Existen **dos alternativas** para representar una red mediante su matriz de adyacencia.\n"
            "La elección dependen de la estructura y el tamaño de la red (número de vértices y enlaces).\n"
            "Dado que la complejidad de una matriz densa es O(n^2), tanto el espacio de almacenamiento\n"
            "como el tiempo de procesamiento pueden resultar prohibitivos en redes de gran escala.\n"
            "Por ello, se han desarrollado técnicas que permiten mantener la fidelidad de la\n"
            "representación minimizando el consumo de recursos.\n"
        
        "\n**Opciones de Impresión**\n\n"
        
        "A continuación, se describen las dos modalidades de visualización disponibles a partir\n"
        "de una instancia de red (archivo de entrada): \n\n");
        
        printf("\t[1] *Impresión completa de la matriz*\n\t"
                     "    Se genera la matriz completa de ceros y unos, donde cada elemento toma el\n\t"
                    "    valor '1' si existe enlace entre dos vértices, y '0' en caso contrario. Esta\n\t"
                    "    representación es exhaustiva, pero en redes con un elevado número de nodos\n\t"
                    "    puede requerir volúmenes de memoria en el orden de gigabytes (por ejemplo,\n\t"
                    "    entre 2 GB y 5B o más), dependiendo del tamaño de la red.\n\n");
        
        printf("\t[2] *Impresión reducida de la matriz*\n\t"
                      "    Se listan únicamente las conexiones existentes, omitiendo las posiciones\n\t"
                      "    correspondientes a enlaces ausentes. Este método acelera los tiempos de\n\t"
                      "    ejecución y disminuye significativamente el uso de memoria, sin sacrificar la\n\t"
                      "    claridad de la topología esencial.\n\n"); 
        
        printf("**Recomendación**\n\n"
            "Para redes de gran dimensión, se aconseja emplear la impresión reducida, ya que\n"
            "optimiza el rendimiento computacional y facilita el análisis de la conectividad. En\n"
            "redes de tamaño moderado o cuando la visualización exhaustiva es prioritaria, la\n"
            "impresión completa sigue siendo una opción válida.\n\n");

} // Fin de la función tipo_de_modo_de_impresion

// Permite mostrar, a través de un archivo de salida específico,
// toda la Matriz de Adyacencia creada anteriormente (i.e. la red)
void crear_matrizAdyacencia_para_todos_los_nodos(ofstream &red) {
    /* 
        A diferencia de imprimir la matriz por filas y columnas y realizar la funcionalidad de "operator<<", 
        utilizaremos "ostream_iterator"  (adaptador de iterador) para encapsular el proceso de escribir 
        elemento por elemento, en un solo paso (reduciendo las llamadas directas y la abstracción de bucles 'for')

        Con ello, podemos hacer que el flujo de salida sea poco más o menos favorable (dependiendo, desde luego, 
        con el tamaño de la red (matriz))
    */
    for (auto &nodo : matriz ) {
        copy(nodo.begin(), nodo.end(), ostream_iterator<int>(red, " "));
        red.put('\n');
    }
} // Fin de la función crear_matrizAdyacencia_para_todos_los_nodos

// Permite mostrar únicamente los nodos que poseen conectividad
// en la red mediante una Matriz de Adyacencia (reduciendo el 
// tamaño y favoreciendo la velocidad del impreso)
void generar_matriz_de_nodos_conectados(ofstream &red) {
    int numero_de_nodos = matriz.size(); // Define el tamaño de la red original
    ostringstream oss;

    // Mediante el conjunto de nodos que posee toda la red
    for ( int u = 0; u < numero_de_nodos; ++u ) {
        // Si el nodo (u) no posee posible conexión con otros nodos 
        // (i.e. no tiene elemento registrado como '1', sino como '0')
        if ( !any_of(matriz[u].begin(), matriz[u].end(), 
            [] ( bool conectado) { return conectado; }) ) {
            continue; // Ignora y continúa con los siguientes nodos (permitiendo reducir el tamaño del impreso)
        }
        // De lo contrario
        oss << (u + 1) << " --> ";                   // Imprime el nodo origen de la red
        int num_nodos_conectados = matriz[u].size(); // Obtiene el número de nodos conectados con nodo (u)

        for ( int v = 0; v < num_nodos_conectados; ++v ) {
            // Si existe conectividad entre nodo (u) y nodo (v)
            if ( matriz[u][v] ) oss << (v + 1) << ' ';         // Imprime todos los nodos conectados hacia el nodo (u)
        }
        oss.put('\n');
    }

    red << oss.str();
} // Fin de la función generar_matriz_de_nodos_conectados


// Genera un archivo de salida (en texto plano) para mostrar gráficamente la red
// mediante una Lista de Adyacencia (constituida por números enteros positivos)
void generar_Lista_Adyacencia(ofstream &red) {
    const int numero_de_nodos = lista.size(); // Asigna el tamaño total de la red

    // Buffer intermedio que almacena toda la salida en memoria y la envía en un único paso
    // al archivo (ofstream), reduciendo llamadas al sistema y mejorando el rendimiento
    ostringstream buffer; 
    buffer << "Nodos   :  Nodos Conectados\n";

    // Dado al número de nodos que hay en la red (lista)
    for ( int nodo = 0; nodo < numero_de_nodos; ++nodo ) {
        buffer << ' ' << (nodo + 1) << "\t\t:   ";
        // En función de sus conexiones (nodos vecinos)
        for ( int conexion_nodo : lista[nodo] ) {
            buffer << (conexion_nodo + 1) << ' '; // Formatea su representación en texto plano
        }
        buffer << '\n';
    }
    // Finalmente
    red << buffer.str(); // Escribe todo en solo paso

} // Fin de la función generar_Lista_Adyacencia


// Determina el grado (número de conexiones) de un nodo asociado a otros nodos de la red.  
// [Nota: no todos los nodos tiene el mismo número de conexiones, dado a su naturaleza no lineal]
int grado_del_nodo_MATRIZ() {
    int total_grado {};            // El total de grado que posee todos los nodos de la red
    int num_nodos = matriz.size(); // Número total de nodos que contiene la red
    nodos.resize(matriz.size());   // Reserva cantidad de memoria basándose al número de nodos que contiene la red

    // Dado al conjunto de nodos que posee la red (nodos (u) y (v))
    for ( int nodo_u = 0; nodo_u < num_nodos; ++nodo_u ) {
        // Calcula el grado (suma el número de conexiones) del nodo (u) hasta los demás nodos
        int grado = accumulate(matriz[nodo_u].begin(), matriz[nodo_u].end(), 0); // "accumulate" registra la suma a partir de los iteradores de un contenedor (realizando operación binaria)
        // Añade a la estructura el nodo de la red y su grado correspondiente
        nodos.push_back( {nodo_u, grado} );
        // Finalmente, suma y asigna el conteo total al número de grado para cada nodo
        total_grado += grado;
    }
    
   return total_grado; 
} // Fin de la función grado_del_nodo_MATRIZ

// Determina el número de conexiones que posee un nodo
// con otros nodos en la red (a partir de una Lista de Adyacencia)
int grado_del_nodo_LISTA() { 
    int total_grado {};           // El número de grado total que posee todos los nodos en la red
    int num_nodos = lista.size(); // Tamaño real de la red
    nodos.resize(lista.size());   // Dado al número de nodos que existe en la red (lista), reserva dicha cantidad en memoria

    // En función al número de nodos
    for ( int nodo_u = 0; nodo_u < num_nodos; ++nodo_u ) {
        int grado = lista[nodo_u].size();   // Determina el número de conexiones
        nodos.push_back( {nodo_u, grado} ); // Añade a la estructura, el nodo y su grado respectivo
        total_grado += grado;               // Posteriormente, calcula el grado total para cada nodo disponible
    }

    return total_grado;
} // Fin de la función grado_del_nodo_LISTA

// Función que retorna la probabilidad de que un nodo 
// tenga un grado específico (siempre con un valor de 1.0)
double obtener_probabilidad_de_grado(int total_grado) {
    double probabilidad = 0.0; // La probabilidad de que un nodo tenga cierto grado
    ostringstream buffer;      // Almacena memoria y reduce llamadas al sistema

    // Dado al número de nodos
    for (const auto &nodo : nodos ) {
        // Calcula su probabilidad, dividiéndolo con el total de grado anteriormente calculado
        probabilidad += static_cast<double> (nodo.grado) / total_grado;
    }
    // Copia toda la salida en un sólo paso
    antSystem << buffer.str();

    return probabilidad;
} // Fin de la función obtener_probabilidad_de_grado


/*
    A continuación, determina qué nodos serán desconectados de la red basado en las métricas y en la búsqueda de 
    soluciones mediante el algoritmo del Sistema Hormiga de tal manera que, al tener una solución (constituida 
    por ceros y unos), nos indiqué que nodos serán separados de la red o del Componente Gigante.

    En este caso, si en la solución contiene el digito uno, se llevará a cabo la desconexión a dicho nodos con los 
    otros nodos que hay en la red original.

    **NOTA**: Tras diversos experimentos y pruebas en diferentes escenarios, he notado que es innecesario "desconectar" 
              realmente la red en función a la estructura de datos seleccionada previamente, ya que, a través del algoritmo 
              de recorrido o búsqueda de profundidad (abreviada como DFS), basta para encontrar los dos componentes más grandes
              (denominados como A y B) que hay en esa red "supuestamente" fragmentada.

              Es decir, la solución (dada por una hormiga, generalmente) es aquél que nos indica qué nodos serán separados 
              completamente de la red. Por lo tanto, se harán únicamente simulaciones a esta fragmentación para reducir espacio
              y acelerar el tiempo computacional a nuestra búsqueda de encontrar la mejor F.O.
*/

// Simula la fragmentación de la red basada en una Matriz de Adyacencia
void red_fragmentada_MATRIZ(const Hormiga &ant, const char *archivo) { 
    // Archivo de salida (separada del resultado de análisis) que muestra
    // cómo la red ha sido fragmentada, dada por cierta solución encontrada
    ofstream fragmentar_red(archivo);
    // Obtiene el tamaño de la red  
    size_t   dimension_de_la_red = matriz.size();  

    // Si no es posible crear el archivo
    // para mostrar la fragmentación de la red
    if ( !fragmentar_red ) {
        cout << "\n\t**ADVERTENCIA**:  Se ha notificado la posibilidad de que el archivo no puede crearse correctamente.\n"
             << "\t\tPor lo tanto, será imposible mostrar satisfactoriamente la fragmentación de la red.\n"
             << "\t\tPor favor, intenta verificar el problema e inténtelo nuevamente.\n\n";

        exit(1); // Finaliza el programa para evitar fallos en escritura de archivos
    }

    // Dado al número de nodos de la red
    /*for ( size_t u = 0; u < dimension_de_la_red; ++u ) {
        for ( size_t v = 0; v < dimension_de_la_red; ++v ) {
            // Verifica qué nodos, dada a la solución, serán desconectados de la red
            bool enlace = ( (!hormiga[id].solucion[u] && !hormiga[id].solucion[v]) ? matriz[u][v] : false );
            fragmentar_red << (enlace ? '1' : '0') << ' ';
        }
        fragmentar_red << '\n';
    }
    
        Aunque esto sea totalmente correcto, ocupa mucho espacio de memoria para poder mostrarlo adecuadamente.
        Lamentablemente, cualquier estructura de datos que ocupa una matriz para representar un sistema (generalmente, una red), 
        siempre tendrá un tiempo computacional o complejidad de O(n^2). Es decir, al momento de trabajar con matrices de adyacencia
        no hay cierto modo de cambiar dicha complejidad (sobre todo, cuando existen redes de mayor tamaño); sin embargo,
        podemos optar alternativas muy concretas para reducir el espacio de memoria y acelerar los tiempos de ejecución.

        Por lo tanto, una alternativa para poder mostrar dicha red en forma de Matriz, es imprimir únicamente aquellos nodos
        que poseen conexión, mientras los otros que no lo poseen, quedan ignorados (reduciendo así el tamaño de dicha matriz). 

        A continuación, se tiene la siguiente alternativa:
    */
    ostringstream buffer;
    buffer << "\n### Cuantificación Formal de la Fragmentación de Red a partir de la Trayectoria del Agente Heurístico" << '\n';
    buffer << '\n' << "Nodo U\tNodo V\n";

    // Dado al conjunto de nodos ((u) y (v)) que posee la red
    for ( size_t u = 0; u < dimension_de_la_red; ++u ) {
        // Si la solución, realizada por una hormiga, 
        // pide desconectar el nodo ('1')
        if ( ant.solucion[u] ) continue; // Omite y continúa el proceso en busca de los nodos activos

        for ( size_t v = 0; v < dimension_de_la_red; ++v ) {
            // Si existe conectividad entre los nodos restantes
            if ( matriz[u][v] ) {
                // Muestra aquellos nodos en el archivo de salida 
                buffer << (u + 1) << "\t\t" << (v + 1) << '\n';
            }
        }
    }
    // Finalmente
    fragmentar_red << buffer.str(); // Escribe todo de golpe

} // Fin de la función red_fragmentada

// Simulación para fragmentar una red mediante una Lista de Adyacencia
void red_fragmentada_LISTA(const Hormiga &ant, const char *archivo) {
    // Archivo de salida (separada del resultado de análisis) que muestra
    // cómo la red ha sido fragmentada, dada por cierta solución encontrada
    ofstream fragmentar_red(archivo); 
    // De igual manera, para incrementar velocidad en la escritura,
    // enviamos en un único paso el archivo (ofstream)
    ostringstream buffer;
    // Obtiene el tamaño real de la red
    int numero_de_nodos = lista.size();

    // Verifica si el archivo creado es correcto
    // para poder mostrar la red fragmentada
    if ( !fragmentar_red ) {
        cout << "\n\t**ADVERTENCIA**:  Se ha notificado la posibilidad de que el archivo no puede crearse correctamente.\n"
             << "\t\tPor lo tanto, será imposible mostrar satisfactoriamente la fragmentación de la red.\n"
             << "\t\tPor favor, intenta verificar el problema e inténtelo nuevamente.\n\n";

        exit(1); // Finaliza el programa para evitar fallos en escritura de archivos
    }
    // De lo contrario
    buffer << "\n### Cuantificación Formal de la Fragmentación de Red a partir de la Trayectoria del Agente Heurístico" << '\n';
    buffer << '\n' << "Nodo (u)  Nodo (v)\n"; // Inicia a Imprimir únicamente los nodos que mantienen conectividad en la red

    // Dado al número de nodos ((u) y (v)) que posee la red (Lista)
    for ( int u = 0; u < numero_de_nodos; ++u ) {
        // Si el nodo (u) es desconectado de la red
        if ( ant.solucion[u] ) continue;   // Ignora y continúa con el resto de los nodos
        // De lo contrario
        buffer << (u + 1) << "  :      ";  // Muestra aquellos nodos que aún son conectados

        // Si el nodo (v) mantiene conexión con nodo (u)
        for ( int v : lista[u] ) { 
            // Muestra todos las conexiones (nodos) que posee el nodo (u)
            buffer.put(' ');
            buffer << (v + 1);
        }
        buffer.put('\n');
    }

    fragmentar_red << buffer.str();
} // Fin de la función red_fragmentada_LISTA

// Determina únicamente tres de todos los componentes conexos posibles que puede haber en la red 
// (generalmente, compleja), los cuales permiten la interacción y el intercambio de información 
// entre los nodos. Mismos componentes son importantes para la operación efectiva y continua de
// la red, de tal manera que la comunicación de ésta misma, ante fallos o ataques, sean mínimas
vector<int> encontrar_componentes_conectados(const Hormiga &ant, int id, short tipo_grafo) {
    // Define el tamaño de la instancia (número de nodos)
    const int tam_de_la_instancia = ant.solucion.size();
    // Los componentes cruciales al momento de fragmentrar toda
    // la red (utilizados mayormente para la evaluación de la F.O.)
    vector<int> componentes; 
    // Reserva memoria para el Componente Separador + 2 componentes grandes (A y B)
    componentes.reserve(3); 
    // Un contador que determina el número total de nodos 
    // que posee el componente o conjunto separador
    int tam_del_separador = 0;

    ostringstream buffer; // Acumula salida en memoria (mejora el rendimiento)
    buffer << "\n>> Registro del agente heurístico (Hormiga Artificial), identificado de manera única mediante su ID <" << (id + 1) << ">\n\n";

    if ( !es_algoritmoED_activado ) buffer << "- Componente Separador |S|: ";
    else                            buffer << "- Componente Separador |S| --> Cardinalidad: ";

    // Dado al número de nodos que serán separados de la red
    for ( int nodo = 0; nodo < tam_de_la_instancia; ++nodo ) {
        // Verifica cuál nodo corresponde al componente separador
        if ( ant.solucion[nodo] ) {
            if ( !es_algoritmoED_activado ) {
                buffer << (nodo + 1) << ' ';

                // Incrementa a uno al contador para obtener el tamaño o número total de nodos del separador
                // y, obtener, además, separación de secuencia en lotes 
                if ( ++tam_del_separador % 33 == 0 ) buffer << '\n' << "\t\t\t\t\t\t\t";
            }
            else {
                ++tam_del_separador;
            }
        }
    }

    if ( !es_algoritmoED_activado ) buffer << "\n\n\t+ Cardinalidad (Número de Nodos) = " << tam_del_separador;
    else                            buffer << tam_del_separador;

    // Escribe todo resultado en el archivo en un solo paso
    antSystem << buffer.str();

    // Posteriormente, agrega el primer componente (conjunto separador)
    componentes.push_back(tam_del_separador);

    antSystem << "\n\n## Identificación de los dos componentes de mayor magnitud (en número de nodos)" << '\n';

    // Busca los dos últimos componentes de mayor tamaño en la red (A y B)
    // según la representación del grafo (Matriz (0) | Lista (1))
    if ( tipo_grafo == 0 )  DFS_MATRIZ(ant, componentes);
    else                    DFS_LISTA (ant, componentes);

    return componentes;
} // Fin de la función encontrar_componentes_conectados

// Realiza el algoritmo denominado como Búsqueda en Profundidad (DFS) para recorrer o buscar nodos 
// a través de grafos (nuestro caso no dirigidos). Dicha exploración, permite encontrar (visitar) 
// aquellos nodos que aún poseen conexiones, de tal forma que es posible determinar los dos 
// componentes más grandes y fundamentales para el uso del sistema hormiga (mejor dicho, para 
// obtener el costo de la función objetivo, que en este caso, corresponde a la maximización)
void DFS_MATRIZ(const Hormiga &ant, vector<int> &componente) {
    vector<bool> nodos_visitados(matriz.size(), false); // Un conjunto de nodos que serán visitados (inicializado en falso)
    int componente_A  = 0; // El tamaño del primer componente mayor (A)
    int componente_B  = 0; // El tamaño del segundo componente igual mayor (B)
    int tam_de_la_red = matriz.size();  // Tamaño original de la red
    /* 
        Definimos arreglos temporales para insertar los nodos para tales
        componentes (en este caso, los de mayor tamaño (A y B)), de tal
        manera que podamos imprimir o mostrar dichos componentes generados
        (si no es utilizado el algoritmo evolutivo)
    */
    vector<int> nodos_actuales; // Todo nodo actual que será visitado posteriormente
    vector<int> A;              // Colección de nodos para el componente A
    vector<int> B;              // Colección de nodos para el componente B

    // Dado al número de nodos que contiene la red (comenzado con el nodo origen (u))
    for ( int u = 0; u < tam_de_la_red; ++u ) {
        // Si el nodo (u) pertenece al conjunto separador o es ya visitado
        if ( ant.solucion[u] || nodos_visitados[u] ) continue;   // Continúa con los siguientes nodos por visitar

        // Inicio del Algoritmo o Recorrido del Grafo (Red)
        deque<int> pila {};         // Estructura auxiliar que contendra los nodos no visitados (o por ser visitados)
        pila.push_back(u);          // Comienza en un nodo raíz cualquier
        nodos_visitados[u] = true;  // Marca como visitado dicho nodo agregado en la pila
        nodos_actuales.clear();     // Borra memoria para cada iteración (para evitar consumir recursos)

        // Mientras los nodos de la red no hayan sido visitados (pila no vacía)
        while ( !pila.empty() ) {
            int nodo_actual = pila.back();          // Extrae aquel nodo que está al tope de la pila
            pila.pop_back();                        // Quita para evitar ciclos no satisfactorios
            nodos_actuales.push_back(nodo_actual);  // Agrega y obtiene el número de nodos que posee el componente

            int nodos_destino = matriz[nodo_actual].size(); // Número de nodos conectados con nodo (u)
            
            // Dado al número de nodos que mantienen conectividad con nodo (u) en la red
            for ( int v = 0; v < nodos_destino; ++v ) {
                // Si está ausente el componente separador (aquellos nodos
                // que son separados totalmente de la red) y no aún sido visitados 
                // y, además, el nodo actual no posee conexiones con nodo (v)
                if ( !ant.solucion[v] && !nodos_visitados[v] && matriz[nodo_actual][v]) {
                    nodos_visitados[v] = true; // Indica que es ya visitado
                    pila.push_back(v);         // Agrega dicho nodo a la pila en busca de otros nodos conectados
                }
            }
            
        } // Fin bucle 'while-do'

        int dimension = nodos_actuales.size(); // La dimensión que contendrá el componente (en busca de dos mayores)

        // Si la dimensión del componente es el mayor de todos
        if ( dimension > componente_A ) {
            componente_B = componente_A; // Intercambia el valor antes encontrado para el segundo
            B            = move(A);      // Mismo movimiento para los vectores (arreglos)
            componente_A = dimension;    // Actualiza su valor para indicar que es el componente con mayor tamaño (después de B)
            A            = nodos_actuales;
        }
        else // De lo contrario, si aún cuando se ha obtenido los dos componentes (A y B)
            if ( dimension > componente_B ) { // Verifica realmente si B es el segundo componente mayor (antes de A)
                componente_B = dimension;
                B            = nodos_actuales;
            }
    } // Fin bucle 'for'

    // Con ello, se ha encontrado los dos componentes que poseen mayor tamaño
    componente.push_back(A.size()); // Añade el componente A en el arreglo en posición 1
    componente.push_back(B.size()); // Añade el componente B en el arreglo en posición 2

    // Si el algoritmo evolutivo no es activado
    if ( !es_algoritmoED_activado ) {
        // Imprime, sin problema alguno, los dos componentes (A y B)
        mostrar_particiones_de_red('A', A);
        mostrar_particiones_de_red('B', B);
    }
    else { // De lo contrario, muestra lo necesario (únicamente el tamaño de los dos componentes grandes)
        antSystem << "\n\t- Componente A --> Cardinalidad:  " << A.size();
        antSystem << "\n\t- Componente B --> Cardinalidad:  " << B.size();
        antSystem.put('\n');
    }

} // Fin de la función DFS_MATRIZ

// Explora (visita) los nodos que poseen conexiones en un grafo no dirigido (la red, en general)
// de tal manera, que al hacer el recorrido o búsqueda por profundidad (DFS) pueda encontrar los dos
// componentes con mayor número de grado [Nota: Mismo algoritmo es utilizado anteriormente con la Matriz,
// pero ahora es con la segunda representación que puede tener una red dada, esto es, a través de la Lista]
void DFS_LISTA(const Hormiga &ant, vector<int> &componente) {
    // Conjunto de nodos que serán visitados (inicializado en falso)
    vector<bool> nodos_visitados(lista.size(), false); 
    int componente_A = 0;              // El tamaño para el primer componente mayor (A)
    int componente_B = 0;              // El tamaño para el segundo componente mayor (B)
    vector<int> actuales;              // Todo nodo actual que indica si es o no visitado
    vector<int> A;                     // Conjunto de nodos que posee el componente A
    vector<int> B;                     // Conjunto de nodos que posee el componente B
    int tam_de_la_red = lista.size();  // El tamaño total de la red

    // A través del número total de nodos que contiene 
    // la red (iniciando con el nodo origen (u))
    for ( int u = 0; u < tam_de_la_red; ++u ) {
        // Si la solución nos pide desconectar el nodo
        // o dicho nodo es ya visitado
        if ( ant.solucion[u] || nodos_visitados[u] ) continue; // Omite y continúa con los nodos siguientes

        // Comienza el algoritmo del Recorrido (DFS)
        deque<int> pila {};         // Estructura **auxiliar** que añadirá los nodos aún sin visitar
        pila.push_back(u);          // Inicia con algún nodo raíz
        nodos_visitados[u] = true;  // Visita el nodo (evita ciclos inesperados)
        actuales.clear();           // Limpia memoria para evitar el consumo de datos innecesarios

        // Mientras todos los nodos de la red no han sido visitados
        while ( !pila.empty() ) {
            int nodo_actual = pila.back();  // Extrae el primer nodo guardado en la estructura 
            pila.pop_back();                // Borra para reducir espacio de memoria (y no generar ciclos infinitos)
            actuales.push_back(nodo_actual); // Añade y obtiene el número de nodos (tamaño) del componente

            // Mediante el conjunto de nodos conectados desde el nodo raíz (nodo (u))
            for ( int v : lista[nodo_actual] ) {
                // Si los nodos no son separados de la red 
                // y aún no han sido visitados
                if ( !ant.solucion[v] && !nodos_visitados[v] ) {
                    nodos_visitados[v] = true; // Visita y continúa con otro nodo diferente
                    pila.push_back(v);         // Añade el nodo para la búsqueda de otros nodos en el componente
                }
            }
        }
        // El número de grado que contendrá el componente (en busca de dos mayores entre todos ellos)
        int dimension = actuales.size();

        // Si la dimensión del componente es mayor que todos
        if ( dimension > componente_A ) {
            componente_B = componente_A; // Intercambia el valor anterior para el segundo componente (B)
            B            = move(A);      // Realiza el mismo intercambio mediante vectores (sin generar coste adicional a través de 'move()')
            componente_A = dimension;    // Actualiza el valor hasta encontrar el componente con mayor tamaño (A)
            A            = actuales;
        }
        else // De otra manera, asegura que el componente B sea el segundo componente con mayor número de grado
            if ( dimension > componente_B ) {
                componente_B = dimension;
                B            = actuales;
            }
    }
    // Quiere decir que se han encontrado, de manera correcta, los dos componentes con mayor tamaño
    componente.push_back(A.size()); // Añade el primer componente ubicado en la posición 1
    componente.push_back(B.size()); // Añade el segundo componente ubicado en la posición 2

    // Si no se ha utilizado el algoritmo evolutivo
    // para calibrar parámetros automáticamente
    if ( !es_algoritmoED_activado ) {
        // Imprime los dos componentes mayores (A y B)
        mostrar_particiones_de_red('A', A);
        mostrar_particiones_de_red('B', B);
    }
    else { // De otra manera, muestra únicamente el número de nodos que posee cada componente grande
        antSystem << "\n\t- Componente A --> Cardinalidad:  " << A.size();
        antSystem << "\n\t- Componente B --> Cardinalidad:  " << B.size();
        antSystem.put('\n');
    }

} // Fin de la función DFS_LISTA

// Sencilla función que muestra las dos particiones con mayor número de grado de la red
void mostrar_particiones_de_red(char id_componente, const vector<int> &componente) {
    // Un contador que permite separar la secuencia de nodos en lotes (inicializado en cero)
    int secuencia_por_lotes = 0; 

    ostringstream oss;
    // Imprime los nodos para cada componente encontrado
    oss << "\n- Componente " << id_componente <<": [ ";

    for ( int nodo : componente ) {
        oss << (nodo + 1) << ' ';
        // Si la secuencia de nodos es un total de 35
        if ( ++secuencia_por_lotes % 35 == 0 ) oss << "\n\t\t\t\t  "; // Separa y ofrece formato adecuado
    }
    oss.put(']');
    oss << "\n\n\t+ Cardinalidad/Tamaño = " << componente.size() << '\n';

    // Por último... Ofrece salida en un solo envío
    antSystem << oss.str();
} // Fin de la función mostrar_particiones_de_red
