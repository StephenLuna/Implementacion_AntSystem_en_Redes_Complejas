// Funciones propias que serán implementadas en este programa (Mecanismo a un Problema de Optimización y
// aplicación al Sistema Hormiga cuyas técnicas ayudarán si la red o grafo es robusto)
#include "Herramientas.h"
#include <iostream>
// Lectura y escritura de archivos
#include <fstream> 
#include <sstream>
// Ordenar una estructura de datos u otros
#include <algorithm> 
// Inicializar y generar 'semilla' para números aleatorios
#include <ctime> 
#include <cstdlib> 
// Operaciones o funciones matemáticos
#include <cmath>
// Estructura de datos tipo cola
#include <deque>
// Biblioteca "thread" para pausar el programa mediante un intervalo de segundos
#include <thread>
// Necesario para utilizar funciones "thread"
#include <chrono> 

using namespace std;

// Variables Globales (Compartidas por todas las hormigas)
float *arregloFeromona; // Toda hormiga tendrá la misma feromona a utilizar
// Las dos maneras de representar un grafo o una red específica:
vector<vector<bool>> matriz {}; 
vector<vector<int>> lista {};
// Se generará un archivo adjunto para facilitar el análisis de los resultados,
// optimizando el proceso de revisión al evitar la ineficiencia asociada con
// la visualización directa en pantalla
ofstream antSystem("valores_AntSystem.txt");
float mejor_costo = 99999.99999f; // El mejor costo de la función objetivo, dado por un número de hormigas
float peor_costo = 0.0f; // El peor costo de la función objetivo mediante el número de hormigas

// Parámetros que serán ajustados a través de un algoritmo evolutivo,
// especificamente Evolución Diferencial, y representados como 
// magnitudes de carácter global:

int num_iteraciones;
double RHO;
double ALPHA;
double BETA;
int m_hormigas;

/*  ** REPRESENTACIÓN COMPUTACIONAL: Instancia del Problema **  */

// Muestra al usuario las diferentes opciones a implementar
short leer_entrada(int argc, char *argv[], string &archivo) {
    short option {}; // Opción para representar un grafo en dos maneras: lista o matriz de adyacencia

    // Verificamos si no está fuera del rango del arreglo o "strings"
    if ( argc < 8 ) { 
        option = -1; // Asignamos el valor "centinela" para terminar la ejecución del programa
        cerr << "\nWARNING: Número insuficiente de argumentos.\n";
        cout << "---------------------------------------------";
        
        // No obstante, mostramos al usuario qué datos de entrada debe ingresar
        cout << "\nArgumentos Requeridos:\n"
             << "\nA. *Parámetros de Control*:\n\n"
             << "\t1. El número de iteraciones (t).\n"
             << "\t2. Rho (estado de evaporación).\n"
             << "\t3. Alpha.\n"
             << "\t4. Beta.\n"
             << "\t5. El número de hormigas (m).\n"
             << "\nNOTA: considera el 'tipo de dato' para cada argumento. En este caso, los argumentos (1 y 5) son números enteros positivos, mientras (2, 3 y 4) son valores con/sin punto decimal.\n"
             << "\nB. *Nombre del Archivo que Contiene los Datos (Nodos y Enlaces) del Grafo* [Incluir, si existe, la extensión. Por ejemplo: \".txt\"].\n"
             << "\nC. *Representación del Grafo (Red)*:\n\n"
             << "\t- Ingresar (0) para utilizar una Matriz de Adyacencia.\n"
             << "\t- Ingresar (1) para utilizar una Lista de Adyacencia.\n\n";
        this_thread::sleep_for(chrono::seconds(16)); // 16 segundos para que el usuario pueda leer la guía de uso

        return option;
    }

    // El algoritmo evolutivo generará las entradas para cada variable de manera heurística,
    // con excepción del nombre del archivo y la opción del grafo a representar
    num_iteraciones = atoi(argv[1]);
    RHO = atof(argv[2]);
    ALPHA = atof(argv[3]);
    BETA = atof(argv[4]);
    m_hormigas = atoi(argv[5]);
    archivo = argv[6];
    option = atoi(argv[7]);

    return option;
} // Fin de la función leer_entrada

// Permite leer correctamente los archivos en formato TSV y construir
// la estrucutura de datos, según su representación (matriz o lista de adyacencia)
void leer_archivo(const string& nombre_archivo, const short tipo_grafo) {
    ifstream archivo(nombre_archivo); // Abre y lee el archivo completo para obtener entrada
    string linea {""}; // La línea que corresponde en cada renglón del archivo (inicialmente vacío)
    // Declara e inicializa el número de nodos y número de enlaces
    int num_nodos {};
    int num_enlaces {};
    int enlaces_repetidos {}; 

    // Si al leer el archivo existe error
    if ( archivo.fail() ) {
        cerr << "Existe un error al abrir el archivo: " << nombre_archivo << "\n";
        exit(EXIT_FAILURE); // Advertimos y salimos del programa sin hacer cambios
    }
    
    // Mientras haya más líneas de texto por leer (almacenar)
    while ( getline(archivo, linea) ) {
        // Ignora si las lineas comienzan con '%'
        if ( linea[0] == '%' ) {
            // Si en la segunda línea contiene metadatos, pero además números en ella
            if ( linea.find_first_of("0123456789") != string::npos ) {
                istringstream stream(linea.substr(1)); // Quita '%' y avanza al siguiente
                stream >> num_nodos >> num_enlaces >> enlaces_repetidos; // Lee y recibe entrada (vértices y aristas)
                
                // Verifica si la representación del grafo es una matriz o lista adyencia (0|1)
                if ( tipo_grafo == 0 ) {
                    matriz = vector<vector<bool>>(num_nodos, vector<bool>(num_nodos, false));
                }
                else {
                    lista.resize(num_nodos);
                }
                continue; // Línea siguiente (Tercera: pares de nodos)
            }
            continue; // Línea siguiente (Segunda: el total de nodos y enlaces)
        }

        // Declara e inicializa los nodos pares, formando como (u, v), según su conexión
        int nodo_origen {}, nodo_destino {};
        istringstream streamParejas(linea); // Nuevamente, extraemos los valores correspondientes

        if ( streamParejas >> nodo_origen >> nodo_destino ) {
            // Dado que los nodos inician en 1 a (n), es necesario convertirlo a indices de 0 a (n - 1)            
            nodo_origen--;
            nodo_destino--;
        }

        // Si alguno de los nodos (u y v) no están dentro del rango esperado
        if ( nodo_origen >= num_nodos || nodo_destino >= num_nodos ) {
            cerr << "ADVERTENCIA: Nodo fuera del rango especificado: ("
                 << nodo_origen + 1 << ", " << nodo_destino + 1 << ")\n";
            exit(EXIT_FAILURE); // Finaliza el programa para no generar posibles errores con la memoria de cada estructura de datos
        }

        // Verifica qué tipo de representación será el grafo a construir
        if ( tipo_grafo == 0 ) {
            // Construimos, finalmente, el grafo no dirigido
            matriz[nodo_origen][nodo_destino] = true;
            matriz[nodo_destino][nodo_origen] = true;
        }
        else {
            // Si entre el nodo origen y el nodo destino, no existe posible lazo (nodo mismo)
            if ( nodo_origen != nodo_destino ) {
                // Transforma la lista de adyacencia en un grafo no dirigido (bidireccional)
                lista[nodo_origen].push_back(nodo_destino);
                lista[nodo_destino].push_back(nodo_origen);
            }      
        }

    } // Fin del ciclo while-do

    // Si la representación del grafo es una lista de adyacencia
    if ( tipo_grafo == 1 ) {
        // Ordena cada elemento o vértice de la lista, según sus conexiones
        for ( int contador = 0; contador < lista.size(); ++contador ) {
            sort(lista[contador].begin(), lista[contador].end());
        }
    }

    archivo.close(); // Cierra el archivo para evitar errores de lectura

} // Fin de la función leer_archivo

// Opción 0: Representación del grafo mediante una MATRIZ
void representacion_matriz(string archivo, int tam_del_grafo) {
    leer_archivo(archivo, 0); // Lee entrada y crea el grafo según la representación correspondiente
    
    // Si al crear el archivo para obtener resultados
    // existe algún fallo o error
    if ( !antSystem ) {
        cout << "\nADVERTENCIA: Se ha detectado un posible error durante la creación y apertura del archivo"
             << " que contiene el conjunto de datos correspondientes a los resultados generados por el algoritmo"
             << " ANT SYSTEM. Se recomienda revisar si el directorio es correcto.\n\n";
        exit(EXIT_FAILURE); // Advierte al usuario y termina el programa inmediatamente
    }

    antSystem << "\n\t>> RED ORIGINAL <<\n";
    imprimir_matriz_adyacencia(matriz); // Muestra al usuario el grafo original
    
    // Iniciamos con la implementación real 
            
    v_nodos nodo; // Arreglo tipo "vector" que reprensenta los nodos de la red y sus datos
    int grado = calcular_grado_Matriz(nodo); // Dado un componente de la red, se calcula el número total del grado 
   
    antSystem << "\tTOTAL DEL GRADO: " << grado << "\n\n";
    // Para cada número del grado de un nodo, se calcula su probabilidad (promedio)
    double probabilidad = probabilidad_grado(nodo, grado);
    antSystem << "\nPROBABILIDAD: " << probabilidad << "\n\n";

    // Damos inicio con la resolución a nuestro Problema de Optimización: Analizar la Robustez de la Red

    tam_del_grafo = matriz.size(); // Asigna el tamaño general del grafo
    creacion_de_feromona(tam_del_grafo); // Inicialmente, toda hormiga tendrá su conjunto de feromona en 1's
    antSystem << "Feromona [*ACTUAL*]: | ";
    mostrar_feromona(tam_del_grafo); // Muestra a pantalla la feromona actual

    antSystem << "\nGenerando Hormigas. Por favor, espere...\n";
    // Aplicamos, a través de ciertas fórmulas, el método hormiga en una iteración (t)
    aplicar_feromona_t(tam_del_grafo, nodo, 0); // 0 corresponde la opción de la MATRIZ

    // Fase de Finalización
    // liberamos memoria a cada estructuras de datos
    matriz.clear();
    nodo.erase(nodo.begin(), nodo.end());
    delete [] arregloFeromona;
    antSystem.close(); // Finalmente, cerramos el archivo de resultados para evitar errores de escritura

} // Fin de la función representacion_matriz

// Opción 1: Representación del grafo mediante una LISTA
void representacion_lista(string archivo, int tam_del_grafo) {
    leer_archivo(archivo, 1); // Pide entrada y crea el grafo como una lista de adyacencia
    
    // Si al crear el archivo para obtener resultados
    // existe algún fallo o error
    if ( !antSystem ) {
        cout << "\nADVERTENCIA: Se ha detectado un posible error durante la creación y apertura del archivo"
             << " que contiene el conjunto de datos correspondientes a los resultados generados por el algoritmo"
             << " ANT SYSTEM. Se recomienda revisar si el directorio es correcto.\n\n";
        exit(EXIT_FAILURE); // Advierte al usuario y termina el programa inmediatamente
    }

    antSystem << "\n\t[ Red Original ]\n";
    imprimir_lista_adyacencia(lista); // Muestra la red original

    // Iniciamos la implementaciòn real del problema

    v_nodos nodo; // Arreglo tipo "vector" que reprensenta los nodos de la red y sus datos
    int grado = calcular_grado_Lista(nodo); // Dado al componente de la red, se calcula el número total del grado 
    antSystem << "\nTOTAL DEL GRADO: " << grado << "\n\n";

    // Para cada número del grado de un nodo, se calcula su probabilidad (promedio)
    double probabilidad = probabilidad_grado(nodo, grado);
    antSystem << "\nPROBABILIDAD: " << probabilidad << "\n";

    // Iniciamos la resolución de nuestro Problema de Optimización: un análisis de una Red Robusta

    tam_del_grafo = lista.size(); // Asigna el tamaño general del grafo (componente de la red)
    creacion_de_feromona(tam_del_grafo); // Inicialmente, toda hormiga tendrá su conjunto de feromona en 1's
    antSystem << "Feromona [*ACTUAL*]: | ";
    mostrar_feromona(tam_del_grafo); // Muestra la feromona actual, antes de hacer cálculos
    
    antSystem << "\nCreando Hormigas, por favor, espere...\n";
    // Aplicamos, a través de ciertas fórmulas, el método hormiga en una iteración (t)
    aplicar_feromona_t(tam_del_grafo, nodo, 1); // 1 corresponde la opción de la LISTA

    // FINAL: Liberar memoria para cada estructura de datos disponibles
    lista.clear();
    nodo.erase(nodo.begin(), nodo.end());
    delete [] arregloFeromona;
    antSystem.close();

} // Fin de la función representacion_lista

// Muestra la red como una lista de adyacencia
void imprimir_lista_adyacencia(const vector<vector<int>> &lista) {
    antSystem << "\nNodos   :  Nodos Conectados\n";
    for ( int nodo = 0; nodo < lista.size(); ++nodo ) {
        antSystem << " " << nodo << "\t:   ";
        for ( int conexion_nodo : lista[nodo] ) {
            antSystem << conexion_nodo << " ";
        }
        antSystem << "\n";
    }

} // Fin de la función imprimir_lista_adyacencia 

// Muestra a pantalla la red mediante una matriz de adyacencia
void imprimir_matriz_adyacencia(const vector<vector<bool>> &matriz) {
    antSystem << "\n";
    for ( int i = 0; i < matriz.size(); ++i ) {
        for ( int j = 0; j < matriz.size(); ++j ) {
            antSystem << matriz[i][j] << " ";
        }
        antSystem << "\n";
    }

} // Fin de la función imprimir_matriz_adyacencia

/*  ** IMPLEMENTACIÓN (RESOLUCIÓN) DEL PROBLEMA: Aplicación del Sistema Hormigas (AS) **   */

// Calcula el grado total de una red o grafo utilizando una matriz de adyacencia y determina el número de grado para cada nodo
int calcular_grado_Matriz(v_nodos &nodo) {
    int grado {}; // Grado Total

    // Dado a un conjunto de nodos (nodo (i) y nodo (j))
    for ( int nodo_i = 0; nodo_i < matriz.size(); ++nodo_i ) {
        int suma_parcial {}; 
        for ( int nodo_j = 0; nodo_j < matriz.size(); ++nodo_j ) {
            // Si entre los mismos nodos no hay conexiones (posibilidad de lazos)
            if ( nodo_i != nodo_j ) {
                // Además, si estos nodos diferentes existe una conexión previa
                if ( matriz[nodo_i][nodo_j] ) {
                    // Obtemos el grado del nodo correspondiente
                    suma_parcial++; 
                }
            }
        } 
        // Añadimos el identificador (número de nodo) y el número de grado para cada nodo
        nodo.push_back({nodo_i, suma_parcial}); 
        // Para cada suma parcial, se obtiene el grado total
        grado += suma_parcial; 
    } 

    return grado;
} // Fin de la función calcular_grado_matriz

// Calcula el grado total de una red o grafo utilizando una lista de adyacencia y determina el número de grado para cada nodo
int calcular_grado_Lista(v_nodos &nodo) {
    int grado {}; // Grado Total

    // Dado a un conjunto de nodos
    for ( int nodo_i = 0; nodo_i < lista.size(); ++nodo_i ) {
        int suma_parcial {};
        for ( int nodo_j : lista[nodo_i] ) { 
            // Al igual que con la matriz, verificamos si no existe lazos en el grafo (o red)
            if ( nodo_i != nodo_j )
                suma_parcial++; // Obtiene el grado correspondiente para cada nodo
        }
        // Añadimos el número de grado a cada nodo de la red
        nodo.push_back({nodo_i, suma_parcial});
        // Sumamos para obtener el grado total
        grado += suma_parcial; 
    }

    return grado;
} // Fin de la función calcular_grado_Lista

// Función para calcular la probabilidad de grado a un conjunto de nodos (componente de la red)
double probabilidad_grado(const v_nodos& nodo, const int grado) {
    double probabilidad {}; // Promedio del Grado

    // Dado al número de nodos o vértices
    for ( const auto &dato_nodo : nodo ) {
        // Muestra el nodo correspondiente y su número de grado respectivo
        antSystem << "NODO " << dato_nodo.ID
                  << "\tNúmero de Grado: " << dato_nodo.grado << "\n";
        
        // Para cada grado de cada vértice, obtenemos su probabilidad, dividiéndolo con el número total de grado
        probabilidad += static_cast<double> (dato_nodo.grado) / grado;
    }

   return probabilidad;
} // Fin de la función probabilidad_grado
 
// Función para obtener la creación de las hormigas y sus datos iniciales
void creacion_de_hormigas(v_hormigas &hormiga, const int tam_universal, const v_nodos &nodo) {
    // Dado que 'hormiga' es un "vector" y no está inicializado, aún cuando es dinámico,
    // pedimos que guarde como elementos 'no activos' una cantidad de "m" hormigas
    hormiga.resize(m_hormigas);

    // Dado un número de hormigas
    for ( int h = 0; h < m_hormigas; ++h ) {
        // Creamos sus datos correspondientes, inicializados en ceros
        hormiga[h].probabilidad = new float [tam_universal] {0.0f};
        hormiga[h].solucion = new bool [tam_universal] {false};
        hormiga[h].listaTabu = new bool [tam_universal] {false};
        hormiga[h].FO =  {0.0f};
    }

} // Fin de la función creacion_de_hormigas

// Función para liberar memoria (datos adicionales) en cada hormiga
void liberar_hormigas(v_hormigas &hormiga) {
    for ( int ant = 0; ant < hormiga.size(); ++ant ) {
        delete [] hormiga[ant].probabilidad;
        delete [] hormiga[ant].solucion;
        delete [] hormiga[ant].listaTabu;
        hormiga[ant].matriz_red.clear();
        hormiga[ant].lista_red.clear();
    }
    hormiga.clear();
} // Fin de la función liberar_hormigas

// Función para crear el conjunto o arreglo de la feromona, inicializados en 1's
void creacion_de_feromona(const int tam_feromona) {
    arregloFeromona = new float [tam_feromona]; // Crea y reserva memoria para el conjunto de la feromona

    for ( int feromona = 0; feromona < tam_feromona; ++feromona ) {
        arregloFeromona[feromona] = 1.0f;
    }

} // Fin de la función creacion_de_feromona

// Muestra a pantalla el arreglo de la feromona
void mostrar_feromona(const int tam_feromona) {
    for ( int feromona = 0; feromona < tam_feromona; ++feromona ) antSystem << arregloFeromona[feromona] << " "; 
    antSystem << "|\n";
} // Fin de la función mostrar_feromona

// Actualiza la feromona, multiplicando (1 - RHO ) [Estado de Evaporación]
void actualizar_feromona(const int tam_feromona, const float RHO) {
    for ( int feromona = 0; feromona < tam_feromona; ++feromona ) {
        arregloFeromona[feromona] *= RHO; // Matemáticamente es: (1 - RHO) X Feromona(t-1)
    }

} // Fin de la función actualizar_feromona

// Ejecuta el método hormiga a partir de un número de iteraciones (t)
void aplicar_feromona_t(const int TAM_RED, v_nodos &nodo, const short opcion) {
    
    antSystem << "\nParametros Equilibrados Requeridos\n"
              << "-----------------------------------------------------\n"
              << "* Número de Hormigas [por crear]: " << m_hormigas
              << "\n* Número de Iteraciones (t-1): " << num_iteraciones
              << "\n* Alpha = " << ALPHA
              << "\n* Beta = " << BETA
              << "\n* Rho = " << RHO << "\n"
              << "-----------------------------------------------------\n\n";

    for ( int t = 0; t < num_iteraciones; ++t ) {
        antSystem << "---Feromona en Iteración t(" << t << ")-->>\n";
        //this_thread::sleep_for(chrono::seconds(4));

        v_hormigas hormiga; // Un conjunto de vectores que representa a las hormigas junto con su información adicional
        creacion_de_hormigas(hormiga, TAM_RED, nodo); // Inicializa o reserva memoria, dado al número de hormigas
        sistema_hormiga(hormiga, TAM_RED, nodo, opcion); // Aplicamos el "método" o Sistema Hormiga
        liberar_hormigas(hormiga);  // Liberamos memoria para evitar datos que son basura
    }

} // Fin de la función aplicar_feromona_t

// A través de la fórmula, calcula la probabilidad de P(i) para cada hormiga 
void calcular_probabilidad_i(v_hormigas &hormiga, int id_hormiga, const v_nodos &nodo, const int TAM_PROBA, float *acumulada) {

    for ( int elemento = 0; elemento < TAM_PROBA; ++elemento ) {
        double tabu = pow(nodo[elemento].grado, BETA); 
        double heuristico = pow(arregloFeromona[elemento], ALPHA);
        double SUMATORIA = Sumatoria_tabu_heuristico(nodo, hormiga[id_hormiga].listaTabu);

        // Si en la lista tabú no tenemos ninguna posición visitada
        if ( hormiga[id_hormiga].listaTabu[elemento] != 1 ) {
            // Calcula probabilididad, que matemáticamente es:
            // P(i) = ( T(i)^BETA * V(i)^ALPHA ) / SUMATORIA[T(u)^BETA * V(u)^ALPHA]
            hormiga[id_hormiga].probabilidad[elemento] = static_cast<float> ( tabu * heuristico ) / SUMATORIA;
        } 
        else {
            // Otro caso, se reemplaza el número de probabilidad en cero para omitirlo
            hormiga[id_hormiga].probabilidad[elemento] = 0.0f;
        }

        // Si el valor actual es el primero del conjunto 
        if ( elemento == 0 ) {
            // Sumamos como 0 + valor de la probabilidad
            acumulada[elemento] += hormiga[id_hormiga].probabilidad[elemento];
        } 
        else { // En otro caso
            // Suma el valor actual más el valor anterior
            acumulada[elemento] += acumulada[elemento - 1] + hormiga[id_hormiga].probabilidad[elemento];
        }

    }

} // Fin de la función calcular_probabilidad_i

// Realiza la suma o sumatoria del denominador en la fórmula de la Probabilidad P(i)
double Sumatoria_tabu_heuristico(const v_nodos &nodo, const bool *lista_tabu) {
    double sumatoria {};
    int contador {}; // Cuenta el número de posiciones de la lista tabú y el arreglo de la feromona

    // Visitamos a cada nodo y pedimos el número de grado correspondiente
    for ( const auto &dato_de_nodo : nodo ) {
        // Si ninguna de las posiciones fue marcado como visitado
        if ( lista_tabu[contador] != 1 )
            // Calculamos la parte del denominador de la Probabilidad (fórmula) con cada elemento de la feromona
            sumatoria += pow(dato_de_nodo.grado, BETA) * pow(arregloFeromona[contador], ALPHA);
        ++contador; // Avanzamos las posiciones siguientes del arreglo (feromona)
    }

    return sumatoria;
} // Fin de la función Sumatoria_tabu_heuristico

// Función que aplica el método hormiga (proceso de buscar una solución)
void sistema_hormiga(v_hormigas &hormiga, const int TAM, v_nodos &nodo, const short opcion) {
    RHO = 1 - RHO; // Calcula el "estado de evaporación" mediante un parámetro equilibrado, denominado como RHO

    // Mediante un conjunto de hormigas
    for ( int h = 0; h < hormiga.size(); ++h ) {
        // Creamos nuestra variable de paro (bandera)
        bool es_listaTabu_llena = false;
        short veces_calculado {};
        float *acumulada = new float[TAM] {0.0f};

        // Repite el proceso (buscar una solución) mientras la lista tabú no esté llena
        while ( !es_listaTabu_llena ) {
            // Si es la primera vez
            if ( veces_calculado == 0 ) {
                // Calculamos de manera normal
                calcular_probabilidad_i(hormiga, h, nodo, TAM, acumulada);
                // Asignamos el valor '1' para continuar con el cálculo (recalculo de la probabilidad)
                veces_calculado = 1; 
            }
            
            // Generamos un número aleatorio entre el 0 y 1 (no necesariamente 0 y 1) a nuestra ruleta simulado
            float probabilidad_aleatoria = static_cast<float> ( rand() ) / RAND_MAX;
            int elegido = encontrar_intervalo(acumulada, probabilidad_aleatoria, TAM);

            // Verifica si en la lista tabú no existe una posición, anteriormente visitado
            if ( hormiga[h].listaTabu[elegido] != 1 ) {
                // Dado a la posición elegida, visitamos esta misma posición al conjunto de solución
                // generando un número aleatorio (completamente heurístico) entre el 0 y 1
                hormiga[h].solucion[elegido] = rand() % 2;
                // Posteriormente, registramos o decimos a nuestra lista tabú, que la posición ya tiene su solución correspondiente
                hormiga[h].listaTabu[elegido] = 1;

                delete [] acumulada;
                acumulada = new float[TAM] {};
                // Volvemos a calcular la probabilidad (P(i)), dado que la posición elegida es igual a cero
                calcular_probabilidad_i(hormiga, h, nodo, TAM, acumulada);
            }
            es_listaTabu_llena = es_llena_la_listaTabu(hormiga, TAM, h); 
        }

        delete [] acumulada;
        antSystem << "\n";

        // Muestra al usuario las soluciones de cada hormiga
        imprimir_solucion_hormiga(hormiga, h, TAM); 
        // Calcula la función objetivo de la hormiga "h"
        funcion_objetivo(hormiga, h, TAM, opcion); 
        // Muestra, de cada hormiga, el valor de la función objetivo
        mostrar_funciones_objetivo(hormiga, h); 

        antSystem << "\t\tProceso de Hormiga " << h + 1 << ": COMPLETADO.\n";
        //this_thread::sleep_for(chrono::seconds(6));
    } 

    // Una vez que las hormigas hayan terminado, actualizamos su feromona 
    actualizar_feromona(TAM, RHO); // Inicializa el "estado de evaporación"

    antSystem << "\n\n...Feromona en su Estado de Evaporación...\n| ";
    mostrar_feromona(TAM);

    // Nuevamente, recorremos el número de hormigas (sin modificar sus datos)
    for (const auto &h : hormiga ) {
        float FO_inversa = 1 / h.FO; // Calcula el inverso de la función objetivo
            
        // Si la solución pertenece al conjunto separador (|S|) (aquellos que están en 1)
        for (int nodo_separador : h.componentes[0] ) {
            // Sumamos el inverso de la función objetivo con la feromona de la hormiga correspondiente
            arregloFeromona[nodo_separador] += FO_inversa;
        }
    }

    antSystem << "\nFeromona [en Estado Inverso de la Función Objetivo]\n| ";
    mostrar_feromona(TAM);
    antSystem << "\n";

} // Fin de la función sistema_hormiga

// Dado un número aleatorio, busca un número aproximado (intervalo correspondiente)
int encontrar_intervalo(float *acumulada, const float NUM_ALEATORIO, const int TAM) {
    int indice_de_solucion {-1};

    for ( int elemento = 0; elemento < TAM; ++elemento ) {
        // Si el número aleatorio (heurístico) está dentro del rango
        if ( NUM_ALEATORIO < acumulada[elemento] ) {
            indice_de_solucion = elemento; // Guardamos la posición y visitamos
            break;
        }
    }

    return indice_de_solucion;
} // Fin de la función encontrar_intervalo

// Función que verifica si una lista tabú está o no llena.
bool es_llena_la_listaTabu(const v_hormigas &hormiga, const int TAM, const int id_hormiga) {
    // Verificamos si en la lista tabú (de una determinada hormiga) 
    // todos los elementos son o no visitados
    for ( int tabu_i = 0; tabu_i < TAM; ++tabu_i ) {
        if ( hormiga[id_hormiga].listaTabu[tabu_i] != 1 ) {
            return false; // Sigue buscando, si es falso
        }
    }

    return true; 
} // Fin de la función es_llena_la_listaTabu

// Muestra el conjunto o arreglo de la probabilidad a una determinada hormiga
void imprimir_probabilidad_i(const v_hormigas &hormiga, const int TAM_PROBA, const int id_hormiga) {
    // Dada una hormiga "h"
    antSystem << "<<Hormiga " << id_hormiga + 1 << ">>\n\tP(i): [";
    for ( int elemento = 0; elemento < TAM_PROBA; ++elemento ) {
        // Imprime su conjunto de probabilidad (P(i))
        antSystem << "" << hormiga[id_hormiga].probabilidad[elemento] << " ";
    }
    antSystem << "]\n";
} // Fin de la función imprimir_probabilidad_i

// Muestra el conjunto solución de cada hormiga
void imprimir_solucion_hormiga(const v_hormigas &hormiga, const int id_hormiga, const int TAM) {
    antSystem << "La Hormiga "<< id_hormiga + 1 << " ha desarrollado una *solución*\n\n";
    antSystem << "[ ";

    for ( int indice = 0; indice < TAM; ++indice ) {
        antSystem << hormiga[id_hormiga].solucion[indice] << " ";
    }
    antSystem << "]\n\n";
} // Fin de la función imprimir_solucion_hormiga


/*  --** RESOLUCIÓN DE UN PROBLEMA DE OPTIMIZACIÓN [ROMPER UNA RED A TRAVÉS DE UNA SOLUCIÓN] --**   */

// Función que descompone una red dada, representada como una "matriz"
void desconectar_red(v_hormigas &hormiga, const int id_hormiga ,const vector<vector<bool>> &matriz, const int TAM) { 
    // Copiamos la red original para evitar que ésta se modifique
    vector<vector<bool>> duplicar_red = matriz;

    // Pedimos la solución correspondiente
    for ( int nodo_i = 0; nodo_i < TAM; ++nodo_i ) {
        // Si en la solución nos pide desconectar la red 
        if ( hormiga[id_hormiga].solucion[nodo_i] == 1 ) {
            // Visitamos todos los nodos al nodo apuntado (conexiones)
            for ( int nodo_j = 0; nodo_j < duplicar_red[nodo_i].size(); ++nodo_j ) {
                // Si el nodo actual tiene una conexión con los demás nodos (vecinos)
                if ( duplicar_red[nodo_i][nodo_j] ) {
                    // Desconectamos y rompemos la red, respectivamente.
                    duplicar_red[nodo_i][nodo_j] = false;
                    duplicar_red[nodo_j][nodo_i] = false;
                }
            }
        }
    }

    // Entretanto, cada hormiga tendría su propia red desconectada
    hormiga[id_hormiga].matriz_red = duplicar_red;
    // Borramos memoria por cada iteración
    duplicar_red.clear(); 

} // Fin de la función desconectar_red

// Función que descompone una red dada, representada como una "lista"
void desconectar_red(v_hormigas &hormiga, const int id_hormiga, const vector<vector<int>> &lista, const int TAM) {
    // Copia la red original para evitar que cada hormiga la modifique
    vector<vector<int>> duplicar_red = lista;

    // Solicitamos la solución para cada una de ellas
    for ( int nodo_i = 0; nodo_i < TAM; ++nodo_i ) {
        // Si en el conjunto solución nos pide desconectar un nodo de la red
        if ( hormiga[id_hormiga].solucion[nodo_i] == 1 ) {
            int nodo_j = 0; // Asignamos el primer nodo dentro de la lista (conexiones del nodo i)
            int tam_original = duplicar_red[nodo_i].size(); // Solicitamos el tamaño original de la red para cada solución

            // Mientras los nodos del "nodo i" estén conectados o existan enlaces de comunicación
            while ( nodo_j < tam_original ) {  
                int nodo_seleccionado = duplicar_red[nodo_i][nodo_j]; // Guardamos el nodo antes de eliminarlo y lo visitamos
                duplicar_red[nodo_i].erase(duplicar_red[nodo_i].begin() + nodo_j); // Posteriormente, Borramos su conexión
                tam_original = duplicar_red[nodo_i].size(); // Una vez eliminado la conexión del nodo, actualizamos el tamaño

                // Dado que la lista de adyacencia es una estructura de datos dinámica...
                // Asignamos su tamaño original en cada iteración (número de nodos a visitar)
                int tam_lista = duplicar_red[nodo_seleccionado].size();
                int numero_posiciones = 0;

                // Itera mientras el número de posiciones (nodos) no sea igual al tamaño de la lista
                while ( numero_posiciones < tam_lista ) {
                    // Si el nodo apuntado es igual a su nodo vecino
                    if ( duplicar_red[nodo_seleccionado][numero_posiciones] == nodo_i ) {
                        // Quitamos la conexión del nodo j a nodo i
                        duplicar_red[nodo_seleccionado].erase(duplicar_red[nodo_seleccionado].begin() + numero_posiciones);

                        // Actualizamos el tamaño de la lista
                        tam_lista = duplicar_red[nodo_seleccionado].size();
                        // Establece el número de posiciones (el nodo a visitar) a cero, como primer elemento
                        numero_posiciones = 0;
                        // Al romper la conexión, salimos para evitar ciclos innecesarios
                        break; 
                    }
                    else { // De otro modo
                        numero_posiciones++; // Continua su proceso hasta terminar (sigue buscando)
                    }

                } // Fin bucle while (INTERNO)

            } // Fin bucle while (EXTERNO)
        }

    } // Fin ciclo for (EXTERNO)

    // Asignamos a cada hormiga su propia red desconectada
    hormiga[id_hormiga].lista_red = duplicar_red;
    // Borramos memoria para evitar fuga de datos
    duplicar_red.clear();

} // Fin de la función desconectar_red

// Función que calcula el conjunto separador de una red
void conjunto_separador(v_hormigas &hormiga, const int id_hormiga ,const int TAM) {
    // Crea los nodos que dependen o crean al conjunto separador |S|
    vector<int> nodo_separador {};

    // Pedimos la solución generada heurísticamente
    for ( int nodo = 0; nodo < TAM; ++nodo ) {
        // Si el nodo corresponde al conjunto separador
        if ( hormiga[id_hormiga].solucion[nodo] == 1 ) {
            // Añade, como primer componente de la red, los nodos desconectados
            nodo_separador.push_back(nodo);
        }
    }

    // Construimos el primer componente (conjunto separador) para c/hormiga
    hormiga[id_hormiga].componentes.push_back(nodo_separador);
    // Borramos memoria
    nodo_separador.erase(nodo_separador.begin(), nodo_separador.end());

} // Fin de la función conjunto_separador

// Función que recorre un grafo o red en profundidad (DFS) "MATRIZ DE ADYACENCIA"
void DFS(vector<bool> &visitado, int nodo, vector<int> &componente, vector<vector<bool>> &red_desconectada) {
    deque<int> pila {}; // Estructura auxiliar tipo "pila"
    
    // Fase de Inicialización
    pila.push_back(nodo); // Comienza agregando un nodo de la red o grafo

    // Fase de Procesamiento
    while ( !pila.empty() ) { // Itera mientras la pila no esté vacía
        // Asignamos el nodo actual de la pila
        int nodo_actual = pila.back();
        pila.pop_back(); // Quitamos, para evitar ciclos

        // Si el nodo actual no fue anteriormente visitado
        if ( !visitado[nodo_actual] ) {
            visitado[nodo_actual] = true; // Marcamos como visitado
            componente.push_back(nodo_actual); // Agrega el nodo, dado al número de componente
            //cout << "\n" << nodo_actual << " Agregado al componente.\n";
            for (int nodo_vecino = 0; nodo_vecino < red_desconectada.size(); ++nodo_vecino ) {
                // Si en la red existe la conexión del nodo y, además, no fue visitado
                if ( red_desconectada[nodo_actual][nodo_vecino] && !visitado[nodo_vecino] ) {
                    //cout << "\nNodo: " << nodo_actual << " conectado con " << nodo_vecino << "\n";
                    pila.push_back(nodo_vecino); // Añade al nodo no visitado a la pila
                }
            }
        }
    } // Fin bucle while

} // Fin de la función DFS

// Determina el número de componentes (si los hay) de la red, excluyendo al conjunto separador
// respetando la representación de la red
void componentes_de_la_red(v_hormigas &hormiga, const int id_hormiga, const short opcion) {
    vector<bool> visitado; // Arreglo auxiliar que ayuda a saber qué nodos son excluidos
    int tam_red {}; // Tamaño general de la red

    // Dada la representación del grafo
    if ( opcion == 0 ) {
        tam_red = hormiga[id_hormiga].matriz_red.size(); // Pedimos el tamaño de la red mediante una MATRIZ
    } 
    else {
        tam_red = hormiga[id_hormiga].lista_red.size(); // Pedimos el tamaño de la red mediante una LISTA
    }

    // Generamos un arreglo de tipo "vector" 
    // para los nodos que serán omitidos al recorrido
    visitado.resize(tam_red, false);

    // Pedimos los nodos que pertenecen al conjunto separador |S|
    for (int nodo_separador : hormiga[id_hormiga].componentes[0] ) {
        // Si el nodo del conjunto separador no fue visitado
        if ( !visitado[nodo_separador] ) {
            visitado[nodo_separador] = true; // Marcamos "visitado" para omitirlo
        }
    }

    // Extrae a cada nodo de la red que anteriormente fue desconectado
    for (int nodo = 0; nodo < tam_red; ++nodo ) {
        // Si el nodo no pertenece al conjunto separador
        if ( !visitado[nodo] ) {
            vector<int> componente {}; // Creamos un nuevo componente, inicializado en cero

            // Según a la representación del grafo
            // Cuenta (crea) el número de componentes que existe en la red a través del recorrido de profundidad
            if ( opcion == 0 ) 
                DFS(visitado, nodo, componente, hormiga[id_hormiga].matriz_red);
            else 
                DFS(visitado, nodo, componente, hormiga[id_hormiga].lista_red);

            // Agrega el componente construido al número de componente, después del conjunto separador
            hormiga[id_hormiga].componentes.push_back(componente); 
        }
    }

    // Liberamos memoria para cada nueva iteración (t)
    visitado.erase(visitado.begin(), visitado.end());
} // Fin de la función componentes_de_la_red

// Función que recorre un grafo o red en profundidad (DFS) [PERO AHORA PARA LA LISTA DE ADYACENCIA]
void DFS(vector<bool> &visitado, int nodo, vector<int> &componente, vector<vector<int>> &red_desconectada) {
    deque<int> pila {}; // Estructura auxiliar tipo "pila"
    
    // Fase de Inicialización
    pila.push_back(nodo); // Comienza agregando un nodo de la red o grafo

    // Fase de Procesamiento
    while ( !pila.empty() ) { // Itera mientras la pila no esté vacía
        // Asignamos el nodo actual de la pila
        int nodo_actual = pila.back();
        pila.pop_back(); // Quitamos, para evitar ciclos

        // Si el nodo actual no fue anteriormente visitado
        if ( !visitado[nodo_actual] ) {
            visitado[nodo_actual] = true; // Marcamos como visitado
            componente.push_back(nodo_actual); // Agrega el nodo, dado al número de componente
           
            for (auto nodo_vecino : red_desconectada[nodo_actual] ) {
                // Si en la red existe un nodo no visitado
                if ( !visitado[nodo_vecino] ) {
                    pila.push_back(nodo_vecino); // Añade al nodo no visitado a la pila
                }
            }
        }

    } // Fin bucle while

} // Fin de la función DFS

// Muestra a pantalla los componentes que existen al momento de romper la red
void mostrar_componentes(const v_hormigas &hormiga, const int id_hormiga) {
    int num_componente = 0;

    antSystem << "\n\t\tHORMIGA " << id_hormiga + 1 << "\n\n";
    for ( auto componente : hormiga[id_hormiga].componentes ) {
        if ( num_componente == 0 )
            antSystem << "* Conjunto Separador |S|: ";
        else
            antSystem << "- Componente " << num_componente << ": ";

        for ( int nodo : componente ) {
            antSystem << nodo << " ";
        }
        antSystem << " Cardinalidad = " << componente.size() << "\n";
        ++num_componente;
    }
    antSystem << "\n";

} // Fin de la función mostrar_componentes

// Calcula la función de costo/objetivo en función del número de componentes de la red y su tamaño para cada hormiga
void funcion_objetivo(v_hormigas &hormiga, const int id_hormiga, const int NODOS_DE_LA_RED, const short opcion) {
    /* Antes de calcular y obtener la función objetivo, es necesario establecer o saber cuáles
    nodos pertenecen al conjunto separador y, posteriormente, determinar el número de componentes
    creados, después de romper la red */

    // PRIMER PASO:
    // Calcular la cantidad de elementos (1) en el conjunto separador (valor base) para cada hormiga "h"
    conjunto_separador(hormiga, id_hormiga, NODOS_DE_LA_RED);

    antSystem << ">>> Dada la [solución], se obtiene la Red Desconectada siguiente >>>\n";

    // SEGUNDO PASO:
    // Dividir la red según los nodos que pertenecen al conjunto separador |S|
    if ( opcion == 0 ) { // Red mediante una MATRIZ DE ADYACENCIA
        desconectar_red(hormiga, id_hormiga, matriz, NODOS_DE_LA_RED);
        imprimir_matriz_adyacencia(hormiga[id_hormiga].matriz_red);
    } 
    else{ // Red mediante una LISTA DE ADYACENCIA
        desconectar_red(hormiga, id_hormiga ,lista, NODOS_DE_LA_RED);
        imprimir_lista_adyacencia(hormiga[id_hormiga].lista_red);
    }

    // TERCER PASO:
    // Construir el número de componentes y sus nodos disponibles (respecto a la representación de la red)
    componentes_de_la_red(hormiga, id_hormiga, opcion);
    // Mostramos el número de componentes creados
    mostrar_componentes(hormiga, id_hormiga);

    /*
    CUARTO PASO: 
    Obtener la función de costo con respecto a la solución de cada hormiga "h"
    */

    // Fase de Inicialización
    int cardinalidad_A = {0}; // Componente A
    int cardinalidad_B = {0}; // Componente B
    int cardinalidad_S {}; // Conjunto Separador (Valor base)
    float nodos_totales {}; // Cálculo de los nodos totales o disponibles
    float porcentaje_valido {}; // Porcentaje que determina si los nodos son o no alcanzados

    // Si el número de componentes es mayor que uno (es decir, si hay más elementos además del conjunto separador)
    if ( hormiga[id_hormiga].componentes.size() > 1 ) {
        // Nos quedamos con los dos componentes más grandes (i.e. |A| y |B|)
        for ( int num_componente = 1; num_componente <= hormiga[id_hormiga].componentes.size() - 1; ++num_componente ) {
            // Si el tamaño del componente es mayor entre los demás componentes de la red
            if ( hormiga[id_hormiga].componentes[num_componente].size() > cardinalidad_A ) {
                cardinalidad_B = cardinalidad_A; // |B| es el segundo componente de tamaño grande
                cardinalidad_A = hormiga[id_hormiga].componentes[num_componente].size(); // |A| es el primer componente con mayor tamaño
            }
            else // De otro modo, si |A| fue siempre mayor
                if ( hormiga[id_hormiga].componentes[num_componente].size() > cardinalidad_B ) {
                    cardinalidad_B = hormiga[id_hormiga].componentes[num_componente].size(); // |B| será el segundo
                }
        }
    }

    // Asignamos el valor base a la variable con base al número de nodos del conjunto separador
    cardinalidad_S = hormiga[id_hormiga].componentes[0].size();
    // Cálculamos los nodos totales, sin contar a los que pertecen al conjunto separador
    nodos_totales = NODOS_DE_LA_RED - cardinalidad_S;
    // Calcula el porcentaje de nodos cubiertos en dos conjuntos, respecto a los nodos disponibles
    porcentaje_valido = (static_cast<float>( cardinalidad_A + cardinalidad_B ) / nodos_totales) * 100;

    antSystem << "|S| = " << cardinalidad_S << "\n";
    antSystem << "|A| = " << cardinalidad_A << "\n";
    antSystem << "|B| = " << cardinalidad_B << "\n";
    antSystem << "- Nodos Totales Válido: " << porcentaje_valido << "\n";

    // Dado a la cardinalidad de los componentes A y B
    // Procedemos a verificar y quedarnos con una función objetivo o costo,
    // de acuerdo a las siguientes condiciones:

    // Si |B| es cero
    if ( cardinalidad_B == 0 ) {
        // Penalizamos, sumando "n" nodos de la red más el costo de la función objetivo (|S|)
        cardinalidad_B = NODOS_DE_LA_RED + cardinalidad_S; 
    }

    // Si |A| es cero
    if ( cardinalidad_A == 0 ) {
        // Penalizamos, sumando "n" nodos de la red más el costo de la función objetivo (|S|)
        cardinalidad_A = NODOS_DE_LA_RED + cardinalidad_S; 
    }

    // Si la suma de |A| y |B| es mayor o igual al 70% de los nodos totales de la red
    if ( porcentaje_valido >= 70 ) {
        hormiga[id_hormiga].FO = cardinalidad_S; // Nos quedamos como función costo el valor base (conjunto separador) "Minimización"
    }
    else { // De lo contrario
        cardinalidad_B = NODOS_DE_LA_RED + cardinalidad_S; // Penalizamos y nos quedamos con la suma de "n" nodos más el costo de la función objetivo (|S|)
        hormiga[id_hormiga].FO = cardinalidad_B; // "Maximización"
    }
    //cout << "\n";

} // Fin de la función "funcion_objetivo"

// Muestra los valores o costo de la función objetivo de cada hormiga
void mostrar_funciones_objetivo(const v_hormigas &hormiga, const int id_hormiga) {
    antSystem << "**Función Objetivo (Costo): " << hormiga[id_hormiga].FO << "\n\n";

    // Además de mostrar cada función objetivo por cada hormiga
    // es necesario obtener, para nuestra sección de resultados,
    // el mejor y el peor costo de la función objetivo

    if ( hormiga[id_hormiga].FO < mejor_costo ) {
        mejor_costo = hormiga[id_hormiga].FO;
    }

    if ( hormiga[id_hormiga].FO > peor_costo ) {
        peor_costo = hormiga[id_hormiga].FO;
    }

} // Fin de la función mostrar_funciones_objetivo

// Una sencilla función que sólo muestra el mejor y el peor costo de una función objetivo
void mostrar_MejorPeor_funcionObjetivo() {
    cout << "\t- Mejor Función Objetivo: " << mejor_costo << "\n";
    cout << "\t- Peor Función Objetivo: " << peor_costo << "\n";
} // Fin de la función mostrar_MejorPeor_funcionObjetivo

