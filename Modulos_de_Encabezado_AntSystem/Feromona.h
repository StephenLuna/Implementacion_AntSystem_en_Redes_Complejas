/*
    "Feromona.h" implementa los mecanismos de gestión del rastro de feromonas usados para
    dirigir el comportamiento colectivo en el algoritmo de colonias de hormigas.
 
    Cada agente deposita feromona en las aristas que recorre; la concentración acumulada
    codifica una preferencia estocástica que incrementa la probabilidad de que rutas de
    alta calidad sean seleccionadas por hormigas posteriores. El módulo incluye operaciones
    de actualización (depositar), evaporación y consulta de niveles de feromona, que son
    esenciales para equilibrar intensificación y diversificación durante la búsqueda de
    caminos óptimos.
*/
#ifndef FEROMONA_H_
#define FEROMONA_H_
using namespace std;

#include "Modelos_de_Datos.h" // Incluye la biblioteca personalizada "Modelos_de_Datos.h" para acceder a datos adicionales de hormigas y nodos
#include <string>

void establecer_linea_de_base_de_feromonas(int intensidad_de_la_ruta); // Configura el sistema de comunicación química, fijando una concentración inicial 
                                                                       // uniforme de feromonas que actúa como estado basal global para todas las hormigas

void imprimir_vector_de_feromonas(string mensaje, const char *nombre_del_archivo); // Permite saber el estado dinámico (inicialización y actualización) de la feromona cuyo rastro
                                                                                   // o guía de comportamiento es utilizado globalmente por todas las hormigas                       

void evaporar_feromona_global(const vector<Hormiga> &hormigas);  // Proceso crucial para evitar converger a óptimos locales (en ella, se da a conocer el parámetro Rho)

void aplicar_rastro_de_feromona(int tam_de_la_instancia, short tipo_grafo); // Ejecuta el algoritmo del Sistema de Hormigas durante (t) iteraciones.
                                                                            // En cada iteración se construyen soluciones por las hormigas, se actualiza
                                                                            // el rastro de feromonas y se evalúa la función objetivo.

/*
    Para favorecer soluciones de menor coste, la cantidad de feromona depositada se hace proporcional al 
    **inverso de la función objetivo**:

                                    [ Inverso(x) = 1 / f(x) ]
    
    Esta actualización se aplica únicamente a los nodos que pertenecen al componente separador (es decir,
    se incluye expresamente el separador), de modo que las soluciones con f(x) pequeña reciben depósitos
    mayores y las soluciones de alto coste obtienen depósitos cercanos a cero y quedan penalizadas.
*/
void   actualizar_feromona_inversa(const vector<Hormiga> &hormigas);

#endif // Fin del archivo FEROMONA_H_
