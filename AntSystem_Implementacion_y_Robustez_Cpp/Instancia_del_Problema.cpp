
/*
    -- Autor: Stephen Luna Ramírez.
    -- Fecha: 17 de agosto a 09 de septiembre de 2025.
    -- Asignatura: Proyecto Terminal (Tesis).
    -- Instructor: Edwin Montes Orozco.
    ============================================================================================================

   **Instancia_del_Problema** designa, de forma precisa y general, la representación computacional de la entrada
    —conjunto de nodos, enlaces, parámetros y estructuras de datos— sobre la cual se formula y resuelve el 
    problema de optimización combinatoria mediante el metaheurístico **Ant System**.
*/


/*
    Bibliotecas con própositos específicos y personalizados 
    para obtener una correcta implementación al programa (Diseño y Desarrollo del Proyecto)
*/

#include "Instancia_del_Problema.h" 
#include "Modelos_de_Datos.h"       // Tipos de datos adicionales que necesita el algoritmo heurístico (Nodos y Hormigas)
#include "Feromona.h"               // Fundamental para la búsqueda de soluciones óptimas. Permite transmitir información (las rutas) y decidir qué caminos son más ideales para todas las hormigas (depende con la cantidad de feromona disponible)
#include "Globales.h"               // Cualquier variable global definida y requerida para ciertas funciones del programa
#include "Grafo.h"                  // Permite recorrer, visitar, desconectar y calcular el número de grado para cada nodo que halla en el grafo (sea matriz o lista)

/*
    Bibliotecas Estándares en Lenguaje de Programación C++
*/

#include <iostream>     // Biblioteca NECESARIA para utilizar todas las funciones estándares de C++ (I/O)
#include <sstream>      // Biblioteca que permite trabajar, eficientemente, con flujos de texto en memoria (manipulación con cadenas de texto o conversión de tipos)
#include <algorithm>    // Biblioteca que proporciona una colección de funciones fundamentales para realizar búsquedas, ordenamientos, conteos, entre otros. Gran soporte de iteradores

/*
    Lee y valida los parámetros de entrada suministrados por el usuario,
    obteniendo las opciones necesarias para la construcción de la red.
    
    A continuación, informa sobre la representación de grafo elegida
    (por ejemplo, matriz de adyacencia o lista de adyacencia), facilitando
    la verificación previa a la construcción y análisis de la topología.
*/

short entrada(int argc, char *argv[]) {
    short opcion = -1; // Opción de representación del grafo (Matriz o Lista de Adyacencia), inicializada con un valor inválido por defecto

    // Si el programa no posee los datos mínimos 
    // para ejecutar el algoritmo heurístico
    if ( argc < 8 ) {
        opcion = -1; // Ofrece un dato no válido para finalizar con la ejecución del programa

        cout << "\n**ADVERTENCIA**: Número insuficiente de argumentos.\n"; 
        cout << "----------------------------------------------------\n";

        // Muestra al usuario qué datos de entrada son necesarios para el programa (Análisis de Robustez) 
        cout << "\n**Argumentos Requeridos**\n"

             << "\n\tA. *Parámetros de Control*:\n\n"
             << "\t\t1. El número de iteraciones (t)\n"
             << "\t\t2. Rho (estado de evaporación)\n"
             << "\t\t3. Alpha (influencia de la feromona)\n"
             << "\t\t4. Beta (influencia heurística)\n"
             << "\t\t5. El número de hormigas (m)\n"

             << "\nNOTA:   Considera el **tipo de dato** para cada argumento.\n"
             << "\tEn este caso, los argumentos (1 y 5) son **números enteros positivos**,\n\tmientras (2, 3 y 4) son **valores con/sin punto decimal**.\n"

             << "\n\tB. *Especificación del fichero que almacena la estructura del grafo o red (Nodos y Enlaces),\n\t"
             << "   indicando su nombre completo, la ruta relativa dentro del directorio 'Casos_de_Estudio' y la extensión\n\t"
             << "   de archivo-si se aplica-(por ejemplo, \".txt\")*. Ejemplo: Casos_de_Estudio/grafo_prueba.txt\n"
             << "\n\tC. *Representación del Grafo (Red)*:\n\n"
             << "\t\t- Ingresar (0) para utilizar una Matriz de Adyacencia.\n"
             << "\t\t- Ingresar (1) para utilizar una Lista de Adyacencia (RECOMENDABLE para redes con un alto número de grado).\n\n";
        
        cout << "Presione la tecla ENTER para continuar\n ";
        fflush(stdout);
        getchar();

        return opcion;
    }
    // De lo contrario, lee, como entrada, los datos del algoritmo:

    // Si ninguno de los datos especificados como parámetros de control del ANT SYSTEM son valores numéricos
    if ( !es_NumeroEntero(argv[1])   || !es_NumeroFlotante(argv[2]) || !es_NumeroFlotante(argv[3]) ||
         !es_NumeroFlotante(argv[4]) || !es_NumeroEntero(argv[5])   || !es_NumeroEntero(argv[7]) ) {
        opcion = -1; // Asigna dato inválido al programa para finalizar su ejecución
        
        cout << "\a\n**ADVERTENCIA:** Ninguno de los **parámetros de control** del Ant System contiene\n"
             << "\t\t un valor numérico válido. Por favor, asigne a cada argumento el tipo de dato\n"
             << "\t\t apropiado (por ejemplo, entero para conteos, real para coeficientes) y vuelva a\n"
             << "\t\t ejecutar el algoritmo.\n\n";

        cout << "Presione la tecla [ENTER] para finalizar...";
        fflush(stdout);
        getchar();

        return opcion; // Envía la señal y finaliza la aplicación sin realizar nada en absoluto
    }

    // Paramétros de Control
    num_iteraciones = atoi(argv[1]);
    RHO             = atof(argv[2]);
    ALPHA           = atof(argv[3]);
    BETA            = atof(argv[4]);
    m_hormigas      = atoi(argv[5]);
    // Nombre del Archivo de Entrada
    instancia       = argv[6];
    // Modelo de Adyacencia (Representación)
    opcion          = atoi(argv[7]);

    // Verifica si alguno de los parámetros de control del Ant System toma un valor negativo
    if ( num_iteraciones <= 0 || RHO < 0 || ALPHA < 0 || BETA < 0 || m_hormigas <= 0 ) {
        opcion = -1; // Indica dato inválido para salir

        cout << "\a\nSe impone la restricción de **no negatividad** en los parámetros de control del algoritmo\n"
             << "Ant System, requisito esencial para garantizar su estabilidad y convergencia. Se ruega\n"
             << "proporcionar valores mayores o iguales a cero (excepto para el número de iteraciones y hormigas) e intentar nuevamente.\n\n";

        cout << "\tPresione la tecla [ENTER] para finalizar...";
        fflush(stdout);
        getchar();

        return opcion; 
    }

    // Verifica si el usuario no proporcionó la instancia del problema (archivo de entrada)
    if ( instancia.empty() )  {
        opcion = -1; // Indica que los datos introducidos no son válidos

        cout << "\a\nNo se detectó ninguna instancia en el archivo de entrada para iniciar el análisis de\n"
             << "robustez de redes mediante el algoritmo Ant System. Por favor, verifique el fichero\n"
             << "de entrada y reintente la ejecución de la implementación.\n\n";

        return opcion; 
    }

    // Si las opciones de grafo no son entre cero y uno
    if ( opcion != 0 && opcion != 1 ) { 
        // Indica que el dato introducido no es válido (y, por tanto, no es posible 
        // representar la red con alguna estructura de datos correspondiente)
        opcion = -1;

        cout << "\a\nLa **opción del grafo** seleccionada no es válida.\n"
             << "Por favor, elige '1' si deseas utilizar una Lista de Adyacencia "
             << "o '0' si prefieres una Matriz de Adyacencia.\n\n";
        
        cout << "\tPresione la tecla [ENTER] para finalizar...";
        fflush(stdout);
        getchar();
    }
    
    return opcion;
} // Fin de la función entrada

// Lee cualquier archivo en formato TSV y construye la estructura de datos
// de acuerdo al diseño original de la red (vértices y enlaces definidos dentro del archivo)
// representándolo a partir de dos posibles formas comúnes: Matriz o Lista de Adyacencia
void leer_archivo(short tipo_grafo) {
    ifstream archivo_entrada(instancia); // Abre el archivo que contendrá la instancia del problema (Topología de la red)
    string   linea {""};                 // Líneas leídas desde el archivo (inicialmente vacío)
    // Cualquier red posee:
    int      num_nodos;         // Un total de número de nodos             
    int      num_enlaces;       // Un número de enlaces (conexiones) entre cualquier nodo existente
    int      enlaces_repetidos; // Dado que las instancias asociando enlaces repetidas, es conveniente incluirla aunque no es utilizada
    /*
        IMPORTANTE: Aunque se procesen parejas de nodos (u, v), es fundamental considerar las conexiones entre ellos.
                    Estas determinan si se ha alcanzado un límite o si aún es posible establecer nuevas conexiones.
                    Si no se toma en cuenta este criterio, pueden generarse nodos aislados, lo que afecta la integridad 
                    de la estructura de datos (ya sea matriz de adyacencia o lista de adyacencia).

                    Esto provoca que el algoritmo visite nodos sin conexiones, generando ineficiencia en el uso de memoria,
                    ya que no se ha evaluado adecuaduamente la cantidad de enlaces existentes entre cada nodo.

                    Para evitar estos problemas, se implementa un contador que determina el punto en el que se debe leer o almacenar la estructura,
                    optimizando el diseño y la creación de la red.
    */
    int      contador_enlaces {};  

    // Si la instancia del problema no es existente
    if ( archivo_entrada.fail() ) {
        cout << "Existe un error al abrir el archivo (INSTANCIA DEL PROBLEMA): " << instancia
             << "\nVerificar e intentar nuevamente para continuar con el **análisis de la red**.\n\n";
        exit(1); // Advierte y finaliza sin realizar alguna acción en particular
    }

    // Mientras el archivo posee más datos para procesar 
    while ( getline(archivo_entrada, linea) ) {
        // Ignora en cualquier inicio para un dato con caracter '%'
        if ( linea[0] == '%') {
            // Si a continuación, el archivo posee metadatos y números contenidos en ella
            if ( linea.find_first_of("0123456789") != string::npos ) {
                istringstream stream(linea.substr(1));                   // Omite el caracter '%' y lee el siguiente dato
                stream >> num_nodos >> num_enlaces >> enlaces_repetidos; // Extrae el número de vértices y enlaces que hay en la red
                
                // Verifica si la estructura de la red es una matriz o lista de adyacencia (0 | 1)
                if ( tipo_grafo == 0 ) matriz = vector<vector<bool>>(num_nodos, vector<bool>(num_nodos, false)); // Asigna el tamaño total de la matriz
                else                   lista.resize(num_nodos);                                                  // Asigna el tamaño general de la lista 

                continue; // Procesa el siguiente dato importante: Pares de nodos
            }
            // De lo contrario
            continue; // Continua con el siguiente dato (en este caso, la segunda línea que contendrá el total de nodos y enlaces)
        }
        // Declara el nodo par (conexión entre nodos (u) y (v))
        int nodo_origen; 
        int nodo_destino;
        istringstream streamParejas(linea); // Extrae los valores para nodo (u) y nodo (v)

        // Si en la red existe la conexión entre de nodos (u) y (v)
        if ( streamParejas >> nodo_origen >> nodo_destino ) {
            // Las estructuras de datos representan estos nodos como índices de 0 a (n - 1) 
            // por lo tanto, dado que las instancias comienzan desde 1 a n
            // Decrementamos a uno para cada uno de ellos y evitamos posibles errores de posición
            --nodo_origen;
            --nodo_destino;
            // Incrementa a uno al contador de enlaces (es decir, existe una conexión)
            ++contador_enlaces;
        }

        // Verifica si alguno de estos nodos ((u) y (v)) introducidos no están fuera del rango
        if ( num_nodos >= 0 && (nodo_origen >= num_nodos || nodo_destino >= num_nodos || nodo_origen < 0 || nodo_destino < 0) ) {
            cout << "\n**ADVERTENCIA**: Nodo fuera del rango especificado: ( "
                 << nodo_origen + 1 << ", " << nodo_destino + 1 << " )\n\n";

            continue; // Advierte y continúa, evitando registrar la arista errónea
        }

        // Dibuja la red en dos posibles formas: Matriz o Lista
        representar_red(tipo_grafo, nodo_origen, nodo_destino);

        // Si el número de enlaces excede a las conexiones 
        // entre los nodos anteriormente registrados (i.e. alcanzó el número esperado)
        if ( contador_enlaces >= num_enlaces ) break; // Finaliza para poder aplicar (correctamente) el algoritmo del Sistema Hormiga

    } // Fin bucle While-Do

    archivo_entrada.close(); // Cierra el archivo para evitar posibles errores en los datos de lectura
    /*
        **Nota**: Tanto aquí, como en la línea 153 del programa o código,
                  eran los conflictos algo confusos con el desarrollo del 
                  programa anterior creado, ya que, dado que no se insertaba 
                  la última arista, hacía que la lista no tuviera la cantidad
                  de memoria solicitada y, además, sin el cierre del archivo,
                  antes de ordenar de dicha lista, generaba problemas para 
                  su ordenamiento correcto.
    */
    
    // Si la selección del grafo pertenece
    // a una Lista de Adyacencia
    if ( tipo_grafo == 1 ) {
        /*
            for ( int node_position = 0; node_position < lista.size(); ++node_position ) 
                sort(lista[node_position].begin(), lista[node_position].end());
        
            El ordenamiento tradicional con 'sort()' en una lista de adyacencia, al parecer,
            no es muy recomendable, no sólo por los tiempos de ejecución que podría costar,
            sino–y aquí es donde ocasionaba mayores problemas para la desconexión de la red
            en el momento de usar el algoritmo o método hormiga (técnica heurística)–, que podía
            existir de nodos duplicados (caso que sucedió con la instancia de Twitter), por lo cual
            generaba malos cálculos o, mejor dicho, evaluaciones de robustez no favorables.

            Por tanto, para evitar de estos detalles, utilizaremos en sí sort(), pero de una manera
            más eficiente (factible):
        */

        // Dado al número de nodos que existe en el grafo (dentro de la lista)
        for ( auto &nodos_u_v : lista ) {
            // Ordena cada par (pareja de nodos) del grafo
            sort(nodos_u_v.begin(), nodos_u_v.end());
            // Pero quita aquellos nodos que son duplicados
            nodos_u_v.erase( unique(nodos_u_v.begin(), nodos_u_v.end()), nodos_u_v.end() );
        }
    }

} // Fin de la función leer_archivo

// Determina y crea el grafo, según la representación del problema
void representar_red(short grafo, int u, int v) {
    // Si entre sus nodos no hay posibilidad 
    // de lazos (nodos repetitivos)
    if ( u != v ) {
        // Si la representación del grafo
        // es una Matriz de Adyacencia 
        if ( grafo == 0 ) matriz[u][v] = matriz[v][u] = true; // Construye el grafo no dirigido
        else { // De lo contrario, es una Lista de Adyacencia
            lista[u].push_back(v);
            lista[v].push_back(u); // Genera, de igual forma, el grafo no dirigido (Lista aún no ordenada)
        }
    }

} // Fin de la función representar_red

// Evaluá la métrica (componentes y características) de robustez a una red compleja (máximo de nodos y enlaces), 
// aplicando el modelado matemático mediante el uso de la Optimización Combinatoria.
// No obstante, buscando la ruptura de la red con una mínima cantidad de nodos (buena solución) guiado por el 
// algoritmo del Sistema Hormiga (Ant System)
void evaluacion_Robustez_redes(short tipo_grafo) {
    leer_archivo(tipo_grafo);  // Recibe los datos de entrada (instancia del problema) y genera el grafo (red) con su representación seleccionada
    /*
        Inicialización de variables (locales) en declaraciones
    */
    int    tam_de_la_red {0};   // El tamaño original de la red (en función de sus nodos), inicialmente vacío
    int    grado         {0};   // Para todo componente de la red, se calcula el número total de grado (número de conexiones)  
    double probabilidad  {0.0}; // En función al número de grado de cada nodo, se obtiene su probabilidad (promedio)

    // Verifica si es posible escribir los resultados 
    // de análisis mediante un archivo de texto 
    if ( !antSystem ) {
        cout << "\n\t**ADVERTENCIA**:  Se ha detectado un posible error durante la creación y apertura del archivo\n"
             << "\tque contiene el conjunto de datos correspondientes a los resultados generados por el algoritmo\n"
             << "\tANT SYSTEM. Se recomienda revisar si el directorio es correcto.\n\n";

        exit(1); // Advierte y finaliza el programa sin realizar nada absolutamente
    }

    // Si la representación de la red es una Matriz de Adyacencia
    if ( tipo_grafo == 0 ) {
        tam_de_la_red = matriz.size();           // Obtiene su tamaño, antes de ser diluida
        grado         = grado_del_nodo_MATRIZ(); // Calcula el número de conexiones que posee un nodo en la red
    }
    else { // De lo contrario, la red es representada mediante una Lista de Adyacencia
        tam_de_la_red = lista.size();           // De igual forma, pide el tamaño original de la red
        grado         = grado_del_nodo_LISTA(); // Posteriormente, calcula el número de conexiones asociadas a ciertos nodos
    }

    probabilidad = obtener_probabilidad_de_grado(grado);                      // Calcula la probabilidad de grado para un cierto nodo
    resultados_del_algoritmo(grado, probabilidad, tam_de_la_red, tipo_grafo); // Escribe los resultados preliminares en el archivo de salida.

    /*
        Ejecuta el Sistema de Hormigas empleando los parámetros de control automatizados y calibrados
        mediante el algoritmo de Evolución Diferencial.

        La ejecución se realiza durante (t) iteraciones; en cada iteración se explora la red para
        evaluar su robustez utilizando la representación seleccionada:

            - Opción 0: Matriz de Adyacencia
            - Opción 1: Lista de Adyacencia
    */
   
    aplicar_rastro_de_feromona(tam_de_la_red, tipo_grafo);

} // Fin de la función crear_red_compleja

void dibujar_red_original(short grafo, int dimension) {
    // Comprueba si la red ha sido generada y representada mediante el archivo de salida,
    // considerando su forma de representación: matriz de adyacencia o lista de adyacencia
    if ( red_activa ) return;            // Sale de la ejecución para evitar repetir el proceso consecutivamente
    else              red_activa = true; // En caso contrario, activa la señal indicando que la red ya fue mostrada en el archivo

    // Si la dimensión de la red no sobrepasa al número máximo de nodos
    if ( dimension <= MAX_NODOS ) {
        // Crea un archivo de salida para mostrar la topología o diseño original de la red (separada de los resultados de análisis)
        ofstream dibujar_red("Resultados_Empiricos_Optimizacion/Estructura_Original_de_la_red.txt");

        // Verifica la posibilidad de error al momento de querer dibujar (construir)
        // la topología de la red original a través del archivo creado
        if ( !dibujar_red ) {
            cout << "\n\t**IMPORTANTE**:  Es probable que exista un error durante la creación del archivo,\n"
                 << "\tel cual contendrá la red en su manera de visualización gráfica.\n"
                 << "\tPor favor, revise el directorio e inténtelo nuevamente.\n\n";

            exit(1); // Advierte y finaliza sin graficar la red en un archivo de texto
        }

        // Es conveniente imprimir todo el funcionamiento (cálculos, grafos, etc.) del sistema
        imprimir_resultado = true;
        dibujar_red << "\n\t\tTopología Original de la Red\n\n";

        // Verifica con qué representación fue creada la red
        // para generar el archivo que contendrá su estructura gráficamente
        if ( grafo == 0 ) generar_Matriz_Adyacencia(dibujar_red);
        else              generar_Lista_Adyacencia (dibujar_red);
    }

} // Fin de la función dibujar_red_original

// Imprime los resultados del algoritmo, incluyendo métricas evaluadas y la estructura original de la red
void resultados_del_algoritmo(int grado, double probabilidad, int dimension_de_la_red, short tipo_grafo) {
    // Crea la red gráficamente en un archivo separado a los resultados posteriores
    dibujar_red_original(tipo_grafo, dimension_de_la_red); 

    antSystem << "\n**Nota**\n\n"
                "Es preciso destacar que el volcado de la estructura original de la red\n"
                "(representación gráfica en texto plano) se generará en un archivo independiente,\n"
                "distinto de aquel que contiene los resultados esenciales del Sistema Hormiga.\n\n"

                "En dicho archivo se incluirán únicamente los detalles imprescindibles para\n"
                "preservar la claridad de los eventos específicos del algoritmo heurístico.\n\n"

                "De igual modo, la fragmentación de la red —resultado de una solución meramente\n"
                "heurística obtenida por un conjunto de hormigas— se visualizará en otro archivo\n"
                "con características similares. Estas medidas se adoptan para simplificar y mejorar\nla legibilidad de los resultados.\n\n"

                "En consecuencia, y siempre que no se emplee el algoritmo de Evolución\nDiferencial, se producirán **seis archivos** en formato '.txt'\n"
                "analizadas en el repositorio identificado como 'Resultados_Empiricos_Optimizacion',\ncorrespondiente al conjunto de datos derivados del proceso de optimización empírica:\n\n"

                    "\t[A]. **Estructura_Original_de_la_Red.txt**\n"
                    "\n\tContiene el número de nodos y las conexiones originales de la red que será\n\tposteriormente analizada.\n\n"

                    "\t[B]. **Desconexion_Estructural_Causada_por_AntSystem/descomposicion_de_red_HORMIGA_<ID>.txt**\n"
                    "\n\tVisualiza la separación de nodos y conexiones conforme a la solución aportada\n\tpor una hormiga determinada.\n\n"
                            
                    "\t[C]. **AntSystem.txt**\n"
                    "\n\tReúne los resultados completos del algoritmo heurístico **Ant System**.\n\n"
                    
                    "\t[D]. **Soluciones_Emergentes_Redes_AntSystem/soluciones_por_hormiga_t<No. de iteración>.txt**\n"
                    "\n\tContiene la solución completa y activa de una hormiga determinada.\n\n"
                    
                    "\t[E]. **rastro_GLOBAL_de_feromonas.txt**\n"
                    "\n\tMuestra o registra las concentraciones de feromona sobre la estructura del grafo (red)\n"
                    "\t(nodos/aristas), constituyendo el mapa base que guía los recorridos de las hormigas.\n\n"
                    
                    "\t[F]. **Estado_Temporal_Feromonas/iteracion_<No. de iteración>_del_registro_de_feromonas.txt**\n"
                    "\n\tEn este archivo se registran las actualizaciones de feromonas realizadas por cada hormiga\n"
                    "\ten una iteración determinada. Cada entrada documenta los recorridos efectuados por las hormigas\n"
                    "\ty las modificaciones resultantes en el rastro de feromonas, incluyendo el efecto de evaporación.\n"
                    "\tEste conjunto de registros permite analizar cómo evolucionan las concentraciones de feromona\n"
                    "\ta lo largo de las iteraciones y facilita la identificación de las rutas más prometedoras.\n\n";
    
    antSystem << "En adelante, se presentan los resultados derivados de la implementación del Sistema Hormiga:\n\n";

    antSystem << ">> Grado global de la red original (número de enlaces): " << grado << '\n';
    antSystem << "\n>> Función de probabilidad del grado para un nodo dado: " << probabilidad << '\n';
    antSystem << "\n\t- Notación Matemática: [P(k_i = k)] donde **k_i** es el grado del nodo 'i'\n";

} // Fin de la función resultados_del_algoritmo


//  ---- Comportamiento adicional del sistema   ----

/*
    Verifica el sistema operativo en ejecución (Linux/MacOS o Windows) 
    con el fin de aplicar el método correcto para limpiar la terminal.

    Esto garantiza que la función de limpieza sea compatible y consistente
    en diferentes entornos
*/

void limpiar_pantalla() {
    #ifdef _WIN32
        system("cls");      // Windows
    #else
        system("clear");    // Sistemas UNIX
    #endif
} // Fin de la función limpiar_pantalla

/*
    Comprueba que el dato introducido por el usuario corresponda efectivamente
    a un valor **numérico entero**. 

    Esta validación previene errores al asegurar que la entrada sea compatible 
    con el procesamiento esperado.
*/

inline bool es_NumeroEntero (const string &dato) {
    stringstream ss(dato); // Buffer interno para operaciones de E/S, equivalente a cin/cout. Útil para conversión y formateo de datos
    int  entero;           // El entero que necesitamos validar
    char verificar_dato;   // Valor proporcionado por el usuario en la entrada

    return ( ss >> entero && !(ss >> verificar_dato) ); // Si el dato es un número entero
} // Fin de la función es_NumeroEntero

// Verifica que el valor ingresado corresponda a un número en punto flotante
inline bool es_NumeroFlotante(const string &dato) {
    stringstream ss(dato);
    double numero;
    char   verificar_dato;

    return ( ss >> numero && !(ss >> verificar_dato) );
} // Fin de la función es_NumeroFlotante