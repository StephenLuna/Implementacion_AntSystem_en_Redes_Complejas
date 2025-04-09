#ifndef HERRAMIENTAS_H_
#define HERRAMIENTAS_H_
// Manipulación a cadena de caracteres
#include <string>
// Arreglos o listas cuya estructura de datos son completamente dinámicos
#include <vector>

// Información adicional para cada nodo
struct Datos_nodo {
    int ID;
    int grado;
};

// Información adicional para cada hormiga 
struct Datos_hormiga {
    bool *listaTabu;
    bool *solucion;
    float *probabilidad;
    float FO;
    // Cada hormiga obtendrá una red desconectada diferente
    // dado a su representación
    std::vector<std::vector<bool>> matriz_red;
    // Definimos una matriz o lista que corresponden a los componentes (su cardinalidad) de la red 
    std::vector<std::vector<int>> componentes;
    std::vector<std::vector<int>> lista_red;
};

using v_nodos = std::vector<Datos_nodo>;
using v_hormigas = std::vector<Datos_hormiga>;

/*  ** FUNCIONES PARA LA REPRESENTACIÓN COMPUTACIONAL: Instancia del Problema **  */ 

// Entrada de Datos
short leer_entrada(int argc, char *argv[], std::string &archivo); 
void leer_archivo(const std::string& nombre_archivo, const short tipo_grafo);
void representacion_matriz(std::string archivo, int tam_del_grafo);
void representacion_lista(std::string archivo, int tam_del_grafo);

// ** LISTA **
void imprimir_lista_adyacencia(const std:: vector<std::vector<int>> &lista);
// *** MATRIZ *** 
void imprimir_matriz_adyacencia(const std:: vector<std:: vector<bool>> &matriz);

/* ** FUNCIONES PARA LA IMPLEMENTACIÓN REAL: Aplicando el Sistema Hormiga ("Ant System") ** */

// << Feromona <<
void creacion_de_feromona(const int tam_feromona);
void mostrar_feromona(const int tam_feromona);
void actualizar_feromona(const int tam_feromona, const float RHO);
void aplicar_feromona_t(const int TAM_RED, v_nodos &nodo, const short opcion);

// \\ Hormigas //
void creacion_de_hormigas(v_hormigas &hormiga, const int tam_universal, const v_nodos &nodo);
void imprimir_probabilidad_i(const v_hormigas &hormiga, const int TAM_PROBA, const int id_hormiga);
void liberar_hormigas(v_hormigas &hormiga);
void sistema_hormiga(v_hormigas &hormiga, const int TAM, v_nodos &nodo, const short opcion);
void imprimir_solucion_hormiga(const v_hormigas &hormiga, const int id_hormiga, const int TAM);

// >> Grado (Lista o Matriz de adyacencia) >>
int calcular_grado_Matriz(v_nodos &nodo);
int calcular_grado_Lista(v_nodos &nodo);
double probabilidad_grado(const v_nodos& nodo, const int grado);

// --- Probabilidad (P(i)) [Fórmula] ---
void calcular_probabilidad_i(v_hormigas &hormiga, int id_hormiga, const v_nodos &nodo, const int TAM_PROBA, float *acumulada);
double Sumatoria_tabu_heuristico(const v_nodos &nodo, const bool *lista_tabu);
int encontrar_intervalo(float *acumulada, const float NUM_ALEATORIO, const int TAM);
bool es_llena_la_listaTabu(const v_hormigas &hormiga, const int TAM, const int id_hormiga);

// ***>>  Problema de Optimización (Resolución)  <<*** 
void conjunto_separador(v_hormigas &hormiga, const int id_hormiga, const int TAM);

// Desconecta la Red como "Matriz de Adyacencia"
void desconectar_red(v_hormigas &hormiga, const int id_hormiga, const std::vector<std::vector<bool>> &matriz, const int TAM);
void DFS(std::vector<bool> &visitado, int nodo, std::vector<int> &componente, std::vector<std::vector<bool>> &red_desconectada);

// Desconecta la Red como "Lista de Adyacencia"
void desconectar_red(v_hormigas &hormiga, const int id_hormiga, const std::vector<std::vector<int>> &lista, const int TAM);
void DFS(std::vector<bool> &visitado, int nodo, std::vector<int> &componente, std::vector<std::vector<int>> &red_desconectada);

// Creación al número de componentes, según la red anteriormente desconectada
void componentes_de_la_red(v_hormigas &hormiga, const int id_hormiga, const short opcion);
void mostrar_componentes(const v_hormigas &hormiga, const int id_hormiga);

// Cálculo de la Función Objetivo
void funcion_objetivo(v_hormigas &hormiga, const int id_hormiga,const int NODOS_DE_LA_RED, const short opcion);
void mostrar_funciones_objetivo(const v_hormigas &hormiga, const int id_hormiga);

#endif
