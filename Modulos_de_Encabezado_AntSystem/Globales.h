/*
    "Configurar_variables.h" centraliza la declaración y definición de variables globales
    necesarias para la ejecución del heurístico Ant System. Estas variables agrupan parámetros
    de control, estado compartido y estructuras auxiliares que son referenciadas por diversos
    módulos del sistema para la configuración y coordinación del algoritmo.
*/
#ifndef GLOBALES_H_
#define GLOBALES_H_

using namespace std;
#include "Modelos_de_Datos.h"
#include <vector>
#include <fstream>
/*
    Variables Globales (visto por todo el código y, por ende, utilizado por toda hormiga)
*/
extern vector<double>       feromona;   // Las hormigas ocuparán la misma feromona para poder llegar a soluciones óptimas
extern vector<vector<bool>> matriz;     // Primera forma de representar una red
extern vector<vector<int>>  lista;      // Segunda forma de representar una red cualquier
extern vector<Nodo>         nodos;      // Representa cualquier nodo que posee la red (tomando en cuenta de sus datos adicionales, véase "Modelos_de_Datos.h")
/*
    Se generará un archivo adjunto para facilitar el análisis de los resultados,
    optimizando el proceso de revisión al evitar la ineficiencia asociada con
    la visualización directa en pantalla
*/ 
extern ofstream  antSystem;                         // Escritura de archivo (mostrar resultados mediante un archivo de salida)
extern ofstream  solucion_final;                    // Representa la solución generada por una hormiga (construida activamente y de manera completa)
extern ifstream  ArchivoResultado_ED;               // Archivo de salida escrita y ocupada únicamente por el algoritmo Evolución Diferencial
extern string    instancia;                         // Nombre de la instancia (entrada de datos)
extern char      nombre_de_archivo_Feromona[300];    // Fichero que representa las actualizaciones previas de feromonas en función al número de hormigas
extern double    costo_optimo_encontrado;           // El mejor costo de la función objetivo (minimización)
extern double    costo_maximo_observado;            // El peor costo de la función objetivo (Maximización) 
// Tres componentes principales para el análisis de Robustez: 
extern const short S; // Componente Separador (|S|)
extern const short A; // Primer  componente grande (A)            
extern const short B; // Segundo componente grande (B)               
/*
    Determina si es necesario mostrar al usuario la totalidad de la ejecución del programa
    considerando que la cantidad de nodos generados no supere un límite predefinido
*/
extern bool      imprimir_resultado;
extern bool      es_algoritmoED_activado;
extern bool      red_activa;
//extern bool      primera_hormiga_activada;
/* 
    El límite establecido para el número de nodos determina la necesidad de registrar
    y presentar todos los resultados al usuario, incluyendo su creación, grado,
    desconexión, entre otros, mediante el archivo generado
*/
extern int MAX_NODOS;
/*
    Parámetros que serán ajustados a través de un algoritmo evolutivo,
    especificamente Evolución Diferencial, y representados como magnitudes 
    de carácter global:
*/ 
extern int      num_iteraciones;
extern double   RHO;
extern double   ALPHA;
extern double   BETA;
extern int      m_hormigas;

#endif // Fin del archivo GLOBALES_H_