
/*
    -- Autor: Stephen Luna Ramírez.
    -- Fecha: 17 de agosto a 09 de septiembre de 2025.
    -- Asignatura: Proyecto Terminal (Tesis).
    -- Instructor: Edwin Montes Orozco.
*/

// Biblioteca personalizada que posee todas las variables globales que serán, 
// posteriormente, utilizadas por todo el código
#include "Globales.h"
// Biblioteca Estándar (I/O)
#include <iostream>

// Define las variables globales (Fase de Inicialización) exactamente una vez

// Estructuras vacías
vector<double>          feromona; 
vector<vector<bool>>    matriz; 
vector<vector<int>>     lista; 
vector<Nodo>            nodos; // Una lista que registra el número de nodos que existen en la red (posible Componente Gigante)

// Archivos (Entrada/Salida)
ofstream antSystem("Resultados_Empiricos_Optimizacion/AntSystem.txt");
ofstream solucion_final;
ifstream ArchivoResultado_ED("indicador_de_progreso_ED.tmp"); // el algoritmo evolutivo establece una métrica de robustez en función al algoritmo hormiga desarrollado previamente
string   instancia   = " ";                                   // La instancia del problema por analizar y resolver
char     nombre_de_archivo_Feromona[300];

// Valores Predefinidas
double    costo_optimo_encontrado =  1e300; // Indicador Inicial
double    costo_maximo_observado  = -1e300;
bool      imprimir_resultado      =  false;
bool      es_algoritmoED_activado =  false;
bool      red_activa              =  false;
// Cuando el algoritmo evolutivo se encuentra habilitado, el sistema permite almacenar hasta 10,000 nodos en los resultados.  
// De lo contrario, se escribe únicamente la información mínima necesaria con el fin de reducir el tiempo de procesamiento  
// durante la búsqueda de soluciones óptimas
int       MAX_NODOS               =  10000; 
// Tres componentes principales para el análisis de Robustez:
const short S = 0; // Componente Separador (Posición 0)
const short A = 1; // Componente (A) (Posición 1)
const short B = 2; // Componente (B) (Posición 2)

// Paramétros de Control (sin datos aún válidos)
int     num_iteraciones = 0;
double  RHO             = 0.0;
double  ALPHA           = 0.0;
double  BETA            = 0.0;
int     m_hormigas      = 0;

