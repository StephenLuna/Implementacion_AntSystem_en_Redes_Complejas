
/*
    **Autor Principal:** Edwin Montes Orozco.
    **Modificaciones Realizadas por:** Stephen Luna Ramírez.
    **Fecha:** 15 y 16 de marzo de 2025.
    **Instructor:** Edwin Montes Orozco.
    **Asignatura:** Proyecto Terminal (Tesis).

    **Objetivo Principal:** Identificar, mediante la aplicación de un algoritmo evolutivo (Evolución Diferencial, DE),
              los parámetros de control óptimos necesarios para la implementación eficiente del Sistema Hormiga (Ant System). 
              Dichos valores serán determinados de manera heurística, maximizando la efectividad y robustez del sistema.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define POP_SIZE 10   //tamanio de la población
#define GEN_MAX 50    //num máximo de generaciones
#define F 0.5         //factor de mutación
#define CR 0.5        //factor de recombinación

//rangos para los parámetros a optimizar
#define MIN_ITER 1
#define MAX_ITER 100
#define MIN_RHO 0.01
#define MAX_RHO 0.99
#define MIN_ALPHA 0.1
#define MAX_ALPHA 5.0
#define MIN_BETA 0.1
#define MAX_BETA 5.0
#define MIN_HORMIGAS 1
#define MAX_HORMIGAS 100

typedef struct{
    int iteraciones;
    double rho;
    double alpha;
    double beta;
    int num_hormigas;
}Individuo;

//generamos un número aleatorio en un rango dado
double random_range(double min, double max){
    return min + ((double)rand() / RAND_MAX) * (max - min);
}

//evaluamos la robustez ejecutando el programa C++ con los parámetros
double evaluar(Individuo ind){
    char comando[500];

    snprintf(comando, sizeof(comando), ".\\robustez.exe %d %lf %lf %lf %d %s %d > resultado.txt", // <-- Aquí cambiar "\\" por "./" si el S.O. es Linux o MacOs
             ind.iteraciones, ind.rho, ind.alpha, ind.beta, ind.num_hormigas, "grafo_prueba.txt", 0); // <-- Incorporar cualquier instancia (archivo) presente en el directorio especificado (llamado "Instacias del Problema")
                                                                                                      // (en este caso particular, se emplea un ejemplo sencillo para propósitos de prueba).
    system(comando);

    //leemos el resultado desde el archivo de salida
    FILE *fp = fopen("resultado.txt", "r");
    if (!fp) return 1e9; //si falla, devolver un valor grande (mala solución)
    double robustez;
    fscanf(fp, "%lf", &robustez);
    //printf("\nROBUSTEZ: %lf\n", robustez);
    fclose(fp);
    return robustez; //asumimos  que el programa devuelve una métrica de robustez (la fo)
}

void evolucion_diferencial() {
    Individuo poblacion[POP_SIZE];
    Individuo mejor;
    double mejor_fitness = 1e9;
   
    //inicializamos la población
    srand(time(NULL));
    for( int i = 0; i < POP_SIZE; i++ ){
        poblacion[i].iteraciones = (int)random_range(MIN_ITER, MAX_ITER);
        poblacion[i].rho = random_range(MIN_RHO, MAX_RHO);
        poblacion[i].alpha = random_range(MIN_ALPHA, MAX_ALPHA);
        poblacion[i].beta = random_range(MIN_BETA, MAX_BETA);
        poblacion[i].num_hormigas = (int)random_range(MIN_HORMIGAS, MAX_HORMIGAS);
    }

    for( int gen = 0; gen < GEN_MAX; gen++ ){
        for( int i = 0; i < POP_SIZE; i++ ){
            //seleccionamos aleatoriamente 3 individuos diferentes
            int a, b, c;

            int max_intentos = 1000; // Establece un valor máximo y arbitrario de intentos
            int intentos = 0; // Contador de intentos

            /* Repite */ do { a = rand() % POP_SIZE; } while (a == i && ++intentos < max_intentos); // si 'a' generó un individuo y el ciclo no fue infinito
            intentos = 0; // reset
            do { b = rand() % POP_SIZE; } while (( b == i || b == a ) && ++intentos < max_intentos); 
            intentos = 0; // reset
            do { c = rand() % POP_SIZE; } while (( c == i || c == a || c == b ) && ++intentos < max_intentos);
            
            if ( intentos >= max_intentos ) { // Si alcanzó el máximo de intentos
                printf("ADVERTENCIA: No se pudieron seleccionar índices válidos\n");
                exit(1); // Decimos que el individuo no fue generado correctamente
            }
        
            //mutacion y recombinación (cruza)
            Individuo trial = poblacion[i];
            if ((rand() / (double)RAND_MAX) < CR) trial.iteraciones = (int)(poblacion[a].iteraciones + F * (poblacion[b].iteraciones - poblacion[c].iteraciones));
            if ((rand() / (double)RAND_MAX) < CR) trial.rho = poblacion[a].rho + F * (poblacion[b].rho - poblacion[c].rho);
            if ((rand() / (double)RAND_MAX) < CR) trial.alpha = poblacion[a].alpha + F * (poblacion[b].alpha - poblacion[c].alpha);
            if ((rand() / (double)RAND_MAX) < CR) trial.beta = poblacion[a].beta + F * (poblacion[b].beta - poblacion[c].beta);
            if ((rand() / (double)RAND_MAX) < CR) trial.num_hormigas = (int)(poblacion[a].num_hormigas + F * (poblacion[b].num_hormigas - poblacion[c].num_hormigas));
            
            /*printf("\nITERACIONES: %d\n", trial.iteraciones);
            printf("\nRHO: %lf", trial.rho);
            printf("\nALPHA: %lf\n", trial.alpha);
            printf("\nBETA: %lf\n", trial.beta);
            printf("\nHORMIGAS: %d\n", trial.num_hormigas);*/

            // Implementamos restricciones específicas con el propósito de evitar valores "excesivamente" elevados
            // que puedan derivar en configuraciones capaces de generar ciclos internos descontrolados.
            // En el presente caso, dichas restricciones se aplican a feromonas que podrían presentar
            // problemas de incompatibilididad.
            // Por tanto, si el valor aleatorio ha llegado al límite, restamos con el valor máximo (y así obtenemos, finalmente, una métrica válida)

            if ( trial.iteraciones < MIN_ITER ) {
                trial.iteraciones = MIN_ITER;
            }
            else if ( trial.iteraciones > MAX_ITER ) {
                int nueva_iteracion = trial.iteraciones - MAX_ITER; 
                trial.iteraciones = MAX_ITER - nueva_iteracion; 
            }

            if ( trial.rho < MIN_RHO ) {
                trial.rho = MIN_RHO;
            }
            else if ( trial.rho > MAX_RHO ) {
                double nueva_rho = trial.rho - MAX_RHO;
                trial.rho = MAX_RHO - nueva_rho;
            }

            if ( trial.alpha < MIN_ALPHA ) {
                trial.alpha= MIN_ALPHA;
            }
            else if ( trial.alpha > MAX_ALPHA ) {
                double nueva_alpha = trial.alpha - MAX_ALPHA;
                trial.alpha = MAX_ALPHA - nueva_alpha;
            }

            if ( trial.beta < MIN_BETA ) {
                trial.beta = MIN_BETA;
            }
            else if ( trial.beta > MAX_BETA ) {
                double nueva_beta = trial.beta - MAX_BETA;
                trial.beta = MAX_BETA - nueva_beta;
            }

            if ( trial.num_hormigas < MIN_HORMIGAS ) {
                trial.num_hormigas = MIN_HORMIGAS;
            }
            else if ( trial.num_hormigas > MAX_HORMIGAS ) {
                int nuevas_hormigas = trial.num_hormigas - MAX_HORMIGAS;
                trial.num_hormigas = MAX_HORMIGAS - nuevas_hormigas;
            }

            //reemplazamos si el nuevo individuo es mejor
            double fitness_trial = evaluar(trial);
            double fitness_actual = evaluar(poblacion[i]);
            if ( fitness_trial < fitness_actual ){
                poblacion[i] = trial;
                if ( fitness_trial < mejor_fitness ){
                    mejor_fitness = fitness_trial;
                    mejor = trial;
                }
            }
        }
    }
    
    //mostramos la mejor configuración encontrada
    printf("Mejores parámetros:\n");
    printf("Iteraciones: %d\n", mejor.iteraciones);
    printf("Evaporación (rho): %lf\n", mejor.rho);
    printf("Alpha: %lf\n", mejor.alpha);
    printf("Beta: %lf\n", mejor.beta);
    printf("Número de hormigas: %d\n", mejor.num_hormigas);
}

int main() {
    printf("\nIniciando optimización con Evolución Diferencial...\n");
    evolucion_diferencial();
    return 0;
}
