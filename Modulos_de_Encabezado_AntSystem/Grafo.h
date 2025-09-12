/*
    Función/archivo encargado del cómputo del grado de cada nodo —entendido como el número total de
    aristas incidentes— en la red original, en función de la representación de grafo adoptada
    (matriz de adyacencia o lista de adyacencia).
 
    Además de calcular los grados, este módulo provee procedimientos de recorrido (p. ej. BFS/DFS u
    otras estrategias) para identificar y simular la desconexión de nodos según soluciones aleatorias
    derivadas del proceso de colonia (dependientes del número de hormigas). Estos recorridos y
    simulaciones permiten estudiar la fragmentación de la red y su robustez utilizando enfoques
    metodológicos como la percolación inversa y el problema del separador de vértices.
 
    Resultado: métricas y estructuras auxiliares necesarias para el análisis de robustez y la toma de
    decisiones sobre remoción/aislamiento de vértices en experimentos heurísticos.
*/
#ifndef GRAFO_H_
#define GRAFO_H_
using namespace std;            

#include "Modelos_de_Datos.h" // Biblioteca personalizada para hacer uso de los datos a ciertos componentes del algoritmo
#include <vector>             // Biblioteca que proporciona estructura de datos lineal en memoria dinámica
#include <fstream>            // Biblioteca que ofrece utilidad para trabajar con archivos, permitiendo leer y escribir datos específicos en ellos

// Tipo de formato para imprimir una red, según su representación
void generar_Lista_Adyacencia(ofstream &red);
void generar_Matriz_Adyacencia(ofstream &red);

// El grado que determina el número de conexiones que hay entre los nodos de la red
int     grado_del_nodo_MATRIZ();
int     grado_del_nodo_LISTA ();
double  obtener_probabilidad_de_grado(int total_grado);

// Desconexión / DFS (Búsqueda en Profundidad (exploración de nodos a un grafo específico)) 
void red_fragmentada_MATRIZ(const Hormiga &ant, const char *archivo); 
void DFS_MATRIZ(const Hormiga &ant, vector<int> &componente); 

void red_fragmentada_LISTA(const Hormiga &ant, const char *archivo);   
void DFS_LISTA(const Hormiga &ant, vector<int> &componente); 

// Al fragmentar cualquier red, éste puede generar (incluso no podría) diversos componentes conexos (subgrafos)
// Sin embargo, sólo nos interesan (de todos ellos) tres: El Separador, y los dos componentes con mayor conectividad (A y B)
std::vector<int> encontrar_componentes_conectados(const Hormiga &ant, int id, short tipo_grafo);
void mostrar_particiones_de_red(char id_componente, const vector<int> &componente);

// Dos posibilidades de mostrar la red en una Matriz de Adyacencia
void tipo_de_modo_de_impresion();
void crear_matrizAdyacencia_para_todos_los_nodos(ofstream &red);
void generar_matriz_de_nodos_conectados(ofstream &red);

#endif // Fin del archivo GRAFO_H_