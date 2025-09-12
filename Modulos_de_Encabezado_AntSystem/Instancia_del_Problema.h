// Este archivo implementa las funciones necesarias para construir la red a partir de un archivo de texto que define la instancia de entrada
#ifndef INSTANCIA_DEL_PROBLEMA_H_
#define INSTANCIA_DEL_PROBLEMA_H_

using namespace std;    // Espacio de nombres para diferenciar bibliotecas estándares

#include <string>       // Biblioteca que permite gestionar o manipular cadena de caracteres
#include <chrono>       // Biblioteca que ofrece una colección flexible de tipos que rastrean el tiempo con distintos grados de precisión (en nuestro caso, 'seconds')
#include <thread>       // Biblioteca que permite la ejecución simultánea de múltiples funciones dentro de una aplicación (nuestro caso, pausar tiempo de ejecución)

// Recibe los argumentos que definen la entrada de datos, es decir, los parámetros de control
short entrada(int argc, char *argv[]);                  
// Lee la instancia del problema con el fin de construir la red correspondiente
void  leer_archivo(short tipo_grafo);                                       
// Según la instancia, selecciona y emplea una estructura de datos lineal específica para representar la red                           
void  evaluacion_Robustez_redes(short tipo_grafo);                                                         
// Representa la red a través de una estructura de datos determinada (Matriz o Lista)
void  representar_red(short grafo, int u, int v);                                                          
// Genera un archivo de texto que describe gráficamente la topología de la red
void  dibujar_red_original(short grafo, int dimension);                                                    
// Muestra los resultados generados por el sistema de optimización basado en hormigas
void  resultados_del_algoritmo(int grado, double probabilidad, int dimension_de_la_red, short tipo_grafo); 

// == Declaración de funciones auxiliares ==

// Limpia toda la información mostrada en la terminal
void  limpiar_pantalla();
/*
    Realiza la validación del valor introducido por el usuario,
    comprobando que corresponda al tipo de dato requerido 
    antes de su uso en el programa:
*/
inline bool es_NumeroEntero  (const string &dato);
inline bool es_NumeroFlotante(const string &dato);

#endif // Fin del archivo INSTANCIA_DEL_PROBLEMA_H_