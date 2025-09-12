// Este archivo declara las estructuras de datos que encapsulan atributos específicos 
// de nodos y agentes (hormigas), fundamentales para el análisis de robustez en redes complejas.
#ifndef MODELOS_DE_DATOS_H_
#define MODELOS_DE_DATOS_H_

/*
    Se recomienda evitar la invocación reiterada del espacio de nombres de la biblioteca estándar
    (std::) al utilizar funciones u objetos. En su lugar, se sugiere declarar el uso del espacio
    de nombres o aplicar alias para mejorar la legibilidad y mantener la consistencia del código
*/
using namespace std;

#include <vector> // Biblioteca para la representación de estructuras de datos lineales dinámicas (pilas, listas simples y arreglos)

// ----   Definición de modelos de datos esenciales que servirán de base para la construcción
//        y correcta implementación del método o algoritmo propuesto   ----

// Cada nodo de la red posee los siguientes campos (datos):
struct Nodo { 
    int ID;     // Un identificador que permite diferenciar entre los demás nodos de la red 
    int grado;  // Un número de grado que representa el número de sus conexiones
};

/*
    Definición de los atributos asociados a cada agente (hormiga).
    
    Estos parámetros son empleados fundamentalmente en el algoritmo heurístico
    para guiar la construcción de soluciones y la exploración del espacio de búsqueda
*/
struct Hormiga {
    vector<bool>            lista_tabu;         // Una lista tabú que permite no ciclarse en regiones locales (movimientos no permitidos)
    vector<bool>            solucion;           // La solución constituida de 0's y 1's que definen la conexión/desconexión de la red
    vector<double>          probabilidad;       // Un conjunto de probabilidades que permite ir de un nodo a otro (favoreciendo el de mayor grado)
    double                  funcion_objetivo;   // El costo general al encontrar una determinada solución (la cual será maximizada o minimizada)
};
   
#endif // Fin del archivo MODELOS_DE_DATOS_H_