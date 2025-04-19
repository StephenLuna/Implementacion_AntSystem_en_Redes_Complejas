/*
    **Autor:** Stephen Luna Ramírez.
    **Fecha:** 8 al 24 de noviembre de 2024. 
    **Modificaciones:** 26 al 30 de noviembre de 2024, 11 a 19 de abril de 2025.
    **Nueva Implementación:** 1 al 13 de diciembre de 2024 (Determinación de la función objetivo para la evaluación de fallos en redes, considerando problemas críticos en la lista).
    **Actualización del Código:** 21 al 29 de diciembre de 2024 (Optimización de la función "sistema_hormiga()" integrándola como una única función de resolución; ajustes derivados en otras funciones).
    **Segunda Implementación:** Consideración cuando los parametros de control son muy altos o cuando el paramétro de control (RHO) generá un valor negativo (11 a 19 de abril de 2025).
    **Instructor:** Edwin Montes Orozco.

    **Objetivo Principal:** Realizar un análisis matemático y computacional de la robustez en redes complejas mediante la implementación del algoritmo basado en el Sistema Hormiga.
*/

#include <iostream> 
#include <vector>
#include <fstream>
// Librería personalizada para la implementación (Problema de Optimización: Robustez en Redes Complejas)
#include "Herramientas.h" 
#include <cstdlib>
#include <algorithm>
#include <ctime>
#include <string>

int main (int argc, char *argv[]) {
    // Nombre del archivo de entrada para construir un grafo específico
    std::string archivo {""};
    // El nombre del archivo que genera el algoritmo evolutivo para establecer la métrica correcta de robustez
    std::ifstream archivo_ED ("resultado.txt");
    // Solicita y recibe la opción de representación del grafo como entrada
    short respuesta_usuario = leer_entrada(argc, argv, archivo);
    // El tamaño general del grafo
    int tam_del_grafo {};
    // Generador (Semilla) de números aleatorios
    srand(time(nullptr)); 

    // Finaliza el programa (ciclo) si el usuario no registró alguna opción válida
    if ( respuesta_usuario == -1 ) {
        // Verifica si el sistema operativo es Linux/MacOs ó Windows
        // para poder limpiar la pantalla correctamente
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif

        return 0;
    }
    // De otro modo, se tiene las siguientes opciones:

    // Si no existe el archivo generado por el algoritmo evolutivo...
    if ( archivo_ED.fail() ) {
        // Quiere decir que el usuario optó por realizarlo manualmente a través de la terminal
        std:: cout << "\n\t- Implementación y Ejecución del Algoritmo ANT SYSTEM - \n\n";
    }

    // Si la opción es cero
    if ( respuesta_usuario == 0 ) { 
        representacion_matriz(archivo, tam_del_grafo); // Representa la red como MATRIZ DE ADYACENCIA
    } 
    else // Otro caso, si la opción es uno
        if ( respuesta_usuario == 1 ) {
            representacion_lista(archivo, tam_del_grafo); // Representa la red como LISTA DE ADYACENCIA
        } 
    
    // Del mismo modo, verificamos si el usuario seleccionó implementarlo a través del algoritmo evolutivo o manualmente
    if ( archivo_ED.fail() ) {
        std:: cout << "\nProceso del método ANT SYSTEM **Finalizado exitosamente**\n\n" 
                   << "\tPor favor, consulte el archivo generado, denominado \"valores_AntSystem.txt\",\n"
                   << "\tpara analizar los resultados obtenidos.\n";

        std:: cout << "\n|------------------------------------------------------------------|\n";
        std:: cout << "\tClasificación Basada en las Funciones Objetivo Evaluadas\n\n";
        mostrar_MejorPeor_funcionObjetivo(false);
        std:: cout << "|------------------------------------------------------------------|\n\n";
    }
    else {
        mostrar_MejorPeor_funcionObjetivo(true); // Si fue con el algoritmo E.D., entonces, sólo generará un valor que corresponde a la métrica de la robustez y, así, generar los parámetros de control
    }

    return 0;
} // Fin de la función main
