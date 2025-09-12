
/*
    -- Autor: Stephen Luna Ramírez.
    -- Fecha: 17 de agosto a 09 de septiembre de 2025.
    -- Asignatura: Proyecto Terminal (Tesis).
    -- Instructor: Edwin Montes Orozco.
    ==================================================================================================================

    **RobustezRedesComplejas_AntSystem** constituye el programa principal de arranque para formular y 
    resolver la instancia del problema sobre un grafo dado. La entrada acepta las representaciones de grafos
    más habituales —matriz de adyacencia y lista de adyacencia— y el objetivo de optimización es determinar la 
    cardinalidad mínima del conjunto separador (los nodos cuya eliminación desconecta la red). Como caso 
    adverso se consideran también las situaciones en que aparecen dos subgrafos de gran tamaño, 
    denominados (A) y (B), cuya aparición corresponde a un coste alto que debe evitarse.

    Para abordar el problema se emplean técnicas heurísticas eficientes: el **Ant System** explora el espacio de 
    soluciones buscando rutas que reduzcan el tamaño del separador, mientras que una segunda 
    metaheurística, **Evolución Diferencial**, se utiliza para calibrar automáticamente los parámetros de control del 
    Ant System (optimización continua de parámetros). Esta combinación pretende mejorar la calidad de las 
    soluciones y reducir los tiempos de ejecución, siempre apoyada en perfiles empíricos que validen las
    decisiones de diseño.

    En una palabra, *RobustezRedesComplejas_AntSystem* es el ejecutable central que orquesta la lectura de la
    instancia, la construcción de soluciones por agentes hormonales, la evaluación de la función objetivo y la
    optimización de parámetros para garantizar escalabilidad y robustez.

    A modo orientativo, el programa requiere —entre otros— **módulos de entrada/salida**, **estructuras de datos 
    para grafos** y **utilidades numéricas**; en C++ esto típicamente incluye cabeceras estándar como <vector>,
    <fstream>, <algorithm>, <cmath>, <numeric>, entre otros; y, opcionalmente bibliotecas especializadas
    (bibliotecas de optimización) según las necesidades de rendimiento y representación.
*/

#include "Instancia_del_Problema.h" // Funciones que ofrecen el mecanismo principal para pedir (leer) entrada y generar o construir la red de acuerdo a la representación del grafo
#include "Globales.h"               // Pide las variables globales que serán utilizadas en ciertas funciones (objetivos concretos del problema)
#include <iostream>                 // Biblioteca **necesaria** para el uso de funciones estándar de C++ (Input/Output)

// La **función principal** utiliza sus argumentos para habilitar la evaluación de robustez
int main ( int argc, char *argv[] ) { 
    /*
        Aunque la función 'rand()' es sencilla, la biblioteca <random> proporciona
        un marco más moderno, seguro y flexible para la generación pseudoaleatoria.

        Recomendaciones:
             - Utilizar motores de alto rendimiento (por ejemplo std::mt19937 o std::mt19937_64).
             - Sembrar con std::random_device o una fuente determinista para reproducibilidad.

        En este proyecto se emplea <random> para garantizar velocidad y robustez en la simulación
        (sobre todo, cuando necesitamos analizar redes con mayor número de grado). Esto se visualiza
        detalladamente en el código fuente "AntSystem.cpp", dentro de la función 'ejecutar_ant_system'
    */  

    // Encargada de solicitar y procesar los datos de entrada: 
    // parámetros del algoritmo heurístico, instancia y configuración del grafo
    short respuesta_del_usuario = entrada(argc, argv); 

    // Si ninguno de los datos introducidos es válido totalmente
    if ( respuesta_del_usuario == -1 ) {
        limpiar_pantalla(); 
        return 1;           // Salimos del programa inmediatamente
    }

    // Verifica si la calibración de parámetros para el sistema hormiga
    // fue mediante el uso del algoritmo evolutivo (Evolución Diferencial)
    if ( ArchivoResultado_ED.fail() ) {
        // Si no fue así, todo será a partir del mismo programa (terminal y archivos de salida)
        cout << "\nAlgoritmo **Ant System** en fase de arranque";
        for ( int punto_suspensivo = 1; punto_suspensivo <= 3; ++punto_suspensivo ) {
            this_thread::sleep_for(chrono::seconds(1));
            cout << '.';
        }
    
        limpiar_pantalla();

        // En ausencia del algoritmo evolutivo, determina la representación a utilizar para construir la red
        if ( respuesta_del_usuario == 0 ) MAX_NODOS = 40000;  // Si es MATRIZ, puede mostrar la red con un total de 40,000 nodos
        else                              MAX_NODOS = 900000; // Si es LISTA,  puede mostrar la red con un número de 900,000 nodos
    } 
    else { // De lo contrario
        es_algoritmoED_activado = true; // Envía señal de activación (el usuario decidió usar Evolución Diferencial)
    }

    /*
        Ejecuta la implementación central del **algoritmo Ant System**, abordando problemas
        de optimización combinatoria mediante técnicas heurísticas y metaheurísticas.

        Este módulo permite evaluar la **robustez** y la **conectividad** de redes complejas,
        generando soluciones aproximadas y métricas que facilitan el análisis estructural
        y funcional de la red.
    */
    evaluacion_Robustez_redes(respuesta_del_usuario);

    /*
        Comprueba la activación del módulo de algoritmo evolutivo (Evolución Diferencial).

        Cuando está habilitado, se presenta únicamente el valor óptimo de la función objetivo
        alcanzado, el cual sirve como métrica de robustez. 
        Este resultado se emplea para ajustar y calibrar de manera óptima los parámetros de control 
        del Ant System, facilitando la búsqueda de soluciones más eficientes y robustas dentro del 
        espacio de búsqueda.
    */

    if ( !es_algoritmoED_activado ) {
        cout << "\n== **Ant System Operativo**: resultados listos para análisis (Consulte la carpeta \"Resultados_Empiricos_Optimizacion\") == \n\n";
        cout << "IMPORTANTE: se ha determinado la función objetivo que representa el valor óptimo --> " << costo_optimo_encontrado << "\n\n";
    }
    else {
        printf("%.17g", costo_optimo_encontrado);
    }

    return 0; //    That's All Folks...!!  ^(^-^)^
}


