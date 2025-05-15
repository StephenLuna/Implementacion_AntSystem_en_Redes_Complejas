
/*
    **Autor Principal:** Edwin Montes Orozco.
    **Modificaciones Realizadas por:** Stephen Luna Ramírez.
    **Fecha:** 15 y 16 de marzo, 19 de abril y 08 de mayo de 2025.
    **Instructor:** Edwin Montes Orozco.
    **Asignatura:** Proyecto Terminal (Tesis).

    **Objetivo Principal:** Identificar, mediante la aplicación de un algoritmo evolutivo (Evolución Diferencial, DE),
              los parámetros de control óptimos necesarios para la implementación eficiente del Sistema Hormiga (Ant System). 
              Dichos valores serán determinados de manera heurística, maximizando la efectividad y robustez del sistema.
*/

#include <stdio.h> // Biblioteca Estándar de C (Input/Output)
#include <stdlib.h> // Biblioteca para la gestión de memoria y otros usos eficientes (por ejemplo, el generador de números aleatorios)
#include <math.h> // Biblioteca que contiene cualquier funcionalidad matematica
#include <time.h> // Necesaria para utilizar 'srand()'
#include <signal.h> // Los macros de señales (y prototipos 'signal()') ayudarán mantener, de manera eficiente, toda información en caso de interrupciones o posibles accidentes
#include <unistd.h> // Biblioteca para uso de unix o posix, tales como el getpid()
#include <stdbool.h> // Biblioteca para utilizar el tipo de dato 'bool' (valores que retornan 'false' o 'true', según la condición)
#include <errno.h> // Para casos de error o posibles fallos inesperados

/*  Valores que no necesitan modificación (i.e. Variables Constantes)  */

#define POP_SIZE 10   // tamaño de la población
#define GEN_MAX 50    // número máximo de generaciones
#define F 0.5         // factor de mutación
#define CR 0.5        // factor de recombinación

// rangos para los parámetros a optimizar
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

// Conjunto de parámetros inherentes a un individuo, indispensables para 
// la implementación del algoritmo 'Ant System'
typedef struct { 
    int iteraciones;
    double rho;
    double alpha;
    double beta;
    int num_hormigas;
} Individuo;

/* 

**Nuevo diseño e implementación** basados en mecanismos de señalización, orientados a optimizar la adquisición
de parámetros del algoritmo 'Ant System'. Dichos mecanismos permiten al usuario evitar tiempos de espera
prolongados al establecer un estado de interrupción, preservando la información previamente registrada
y posibilitando la reanudación del proceso en un momento posterior si pérdida de datos.

*/

// Función para la preservación de datos del 'individuo', evitando la pérdida de información en caso
// de suspensión o interrupción. Permite al usuario finalizar la ejecución del programa y reiniciar
// posteriormente sin necesidad de repetir el proceso de inicialización.
// Diseñada para manejar eficientemente grandes volúmenes de datos asociados al conjunto de nodos y enlaces
void checkpoint(Individuo poblacion[], int poblacion_actual, int generacion_actual, double mejor_fitness, Individuo mejor) {
    FILE *save_file = fopen("CHECKPOINT.txt", "w"); // Crea y abre el archivo en operación de escritura para guardar los datos

    if ( !save_file ) { // Verifica si el archivo no está del todo creado correctamente
        printf("\nSe ha detectado un error en la apertura y escritura del archivo \"CHECKPOINT.txt\", impidiendo el almacenamiento de información relevante.\n");
        return;
    }

    /* Inicia el autoguardado: */ 

    // Guarda en forma segura, la generación actual (primera iteración) y el mejor fitness
    fprintf(save_file, "%d %d %lf\n", generacion_actual, poblacion_actual, mejor_fitness);
    // Guarda, de igual forma, todo dato del mejor individuo (mejores parámetros [para ese momento])
    fprintf(save_file, "%d %lf %lf %lf %d\n", mejor.iteraciones, mejor.rho, mejor.alpha, mejor.beta, mejor.num_hormigas);
    // Finalmente, guardamos también toda la población generada (sin ella, no podemos realizar la mutación y la cruza)
    for ( int i = 0; i < POP_SIZE; ++i ) {
        fprintf(save_file, "%d %lf %lf %lf %d\n", poblacion[i].iteraciones, poblacion[i].rho,
                poblacion[i].alpha, poblacion[i].beta, poblacion[i].num_hormigas);
    }
    fclose(save_file); // Cerramos el archivo para asegurarnos que no se debe escribir nada en absoluto

} // Fin de la función checkpoint

// Gracias al mecanismo de autoguardado, el usuario puede **reiniciar** la ejecución del programa conservando los datos
// previamente obtenidos. Esto permite la **recuperación** de la información del 'individuo' para la optimización
// de parámetros, considerando el número de generaciones y la población establecida. Además, se incorpora el análisis
// de la red a explorar y evaluar mediante los métodos "Ant System", "VSP" y "Percolación Inversa".
bool cargar_checkpoint(Individuo *poblacion, int *poblacion_actual, int *generacion_actual, double *mejor_fitness, Individuo *mejor) {
    FILE *load_file = fopen("CHECKPOINT.txt", "r"); // Abre el archivo con los datos guardados en su operación de lectura

    // Si no existe (no se creó ningún checkpoint o autoguardado)
    if ( !load_file ) return 0; // Decimos que no hay ningún dato guardado disponible

    /* Se verifica la **existencia** y **disponibilidad** de los datos esenciales, asegurando que han sido correctamente almacenados.
       Esto permite la continuidad del proceso sin interrupciones, garantizando la obtención óptima de los parámetros de control del
       algoritmo 'Ant System' en función de la información generada previamente 
    */

    if ( fscanf(load_file, "%d %d %lf", generacion_actual, poblacion_actual, mejor_fitness) != 3 ) {
        fclose(load_file);
        return 0;
    }

    if ( fscanf(load_file, "%d %lf %lf %lf %d", &(mejor->iteraciones), &(mejor->rho), &(mejor->alpha), &(mejor->beta), &(mejor->num_hormigas)) != 5 ) {
        fclose(load_file);
        return 0;
    }

    for ( int i = 0; i < POP_SIZE; ++i ) {
        if ( fscanf(load_file, "%d %lf %lf %lf %d", &poblacion[i].iteraciones, &poblacion[i].rho,
                    &poblacion[i].alpha, &poblacion[i].beta, &poblacion[i].num_hormigas) != 5 ) {
            fclose(load_file);
            return 0;
        }
    }

    fclose(load_file); // Cierra el archivo, después de leer todos los datos 
    return 1; // Por tanto, decimos que el "checkpoint" (el autoguardado) fue cargado exitosamente

} // Fin de la función cargar_checkpoint

// Elimina por completo el contenido visualizado en la terminal del usuario,
// adaptándose al sistema operativo en uso.
void limpiar_pantalla() {
    // Verifica si el sistema operativo del usuario
    // pertenece a Windows o Linux/MacOs para poder limpiar la pantalla correctamente
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
} // Fin de la función limpiar_pantalla

// generamos un número aleatorio en un rango dado
double random_range(double min, double max) {
    return min + ((double)rand() / RAND_MAX) * (max - min);
} // Fin de la función random_range

// evaluamos la robustez ejecutando el programa C++ con los parámetros
double evaluar(Individuo ind) {
    char comando[800];
    // **NOTA:** En el caso de que el Sistema Operativo sea Windows, la instrucción debe iniciarse con la sintaxis: ".\\nombre_del_ejecutable.exe".
    // Por el contrario, si el sistema operativo corresponde a Linux o macOs, la instrucción debe adaptarse a la forma "./nombre_del_ejecutable"
    snprintf(comando, sizeof(comando), ".\\robustez.exe %d %lf %lf %lf %d %s %d > resultado.txt",
             ind.iteraciones, ind.rho, ind.alpha, ind.beta, ind.num_hormigas, "grafo_prueba.txt", 0); // <-- Incorporar cualquier instancia (archivo) presente en el directorio especificado (llamado "Instacias del Problema")
                                                                                                      // (en este caso particular, se emplea un ejemplo sencillo para propósitos de prueba).
    system(comando);
    
    // Leemos el resultado desde el archivo de salida
    FILE *fp = fopen("resultado.txt", "r");
    if (!fp) return 1e9; // si falla, devolver un valor grande (mala solución)
    double robustez;
    fscanf(fp, "%lf", &robustez);
    fclose(fp);
    return robustez; // asumimos  que el programa devuelve una métrica de robustez (la f.o.)

} // Fin de la función evaluar

// Variables Globales
volatile bool detener_evolucionDiferencial = false; // El propio compilador sabrá que esta variable será modificada para casos externos (como Hardware e interrupciones)
static short contador_ctrlC = 0; // Un contador de interrupciones cuando la señal de suspensión haya sido notificado a través del usuario (inicializado en cero)

// Prototipo y diseño general de la función "signal()", encargada de detectar la interrupción del programa mediante
// la combinación de teclas (CTRL + C). Al activarse, establece un estado lógico verdadero, permitiendo la ejecución
// del procedimiento de guardado de datos para garantizar la preservación de la información del usuario de manera satisfactoria.
void apagar_evaluacion(int sig) {
    contador_ctrlC++; // Suma uno al contador para determinar las veces que ha sido registrado (enviado)
    signal(SIGINT, apagar_evaluacion);
    detener_evolucionDiferencial = true;
} // Fin de la función apagar_evaluacion

// Identificación de los parámetros óptimos del algoritmo 'Ant System' para maximizar la eficiencia 
// en la optimización de la función objetivo. Se aplica el enfoque de Evolución Diferencial,
// adaptándolo a la naturaleza discreta del problema, a pesar de su uso tradicional en la **optimización continua**.
void evolucion_diferencial() {
    // Definimos mejor una estructura de datos lineal dinámica para poder guardar sus datos relevantes en el archivo
    Individuo *poblacion =  malloc(POP_SIZE * sizeof(Individuo)); 
    Individuo mejor;
    double mejor_fitness = 1e9;
    // El número de generación que será modificado en cada proceso (evaluación e iteración), 
    // inicializado en cero (que corresponde la primera generación)
    int generacion_actual = 0;
    // De igual manera, el número de población, la cual será modificado por un número de iteraciones
    // (inicializado, si es la vez primera, en cero)
    int poblacion_actual = 0; 
    srand(time(NULL)); // Inicializamos el generador aleatorio

    // Verifica si el usuario guardó su proceso más reciente (i.e si existe algún "checkpoint")
    if ( cargar_checkpoint(poblacion, &poblacion_actual, &generacion_actual, &mejor_fitness, &mejor) ) {
        printf("\n\t**Checkpoint cargado exitosamente**. Procediendo con la reanudación desde la **generación (%d)** y **población (%d)**. Espere mientras se completa el proceso.\n", generacion_actual, poblacion_actual);
        sleep(5); // Ofrece tiempo para leer el aviso
    }
    else { // Otro caso, inicializa la población desde cero
         for( int i = 0; i < POP_SIZE; ++i ) {
            // Esto es, asigna todos los valores iniciales a cada individuo
            poblacion[i].iteraciones = (int)random_range(MIN_ITER, MAX_ITER);
            poblacion[i].rho = random_range(MIN_RHO, MAX_RHO);
            poblacion[i].alpha = random_range(MIN_ALPHA, MAX_ALPHA);
            poblacion[i].beta = random_range(MIN_BETA, MAX_BETA);
            poblacion[i].num_hormigas = (int)random_range(MIN_HORMIGAS, MAX_HORMIGAS);
        }
    }
    
    // Establece (instala) la rutina "apagar_evaluacion()" como manejador de señal INT (CTRL + C)
    signal(SIGINT, apagar_evaluacion);

    for ( int gen = generacion_actual; gen < GEN_MAX; ++gen ) { // Inicia las generaciones (la llamada a la función objetivo)
        printf("\nNúmero de Generación: %d | ", gen + 1);
        for ( int i = poblacion_actual; i < POP_SIZE; ++i ) { // Para cada generación, se obtiene una población (mejora o rechazo)
            printf("Número de Población: %d\n", i + 1);

            int a, b, c; // Seleccionamos aleatoriamente 3 individuos diferentes
            int max_intentos = 1000; // Establece un valor máximo y arbitrario de intentos
            int intentos = 0; // Contador de intentos

            /* Repite */ do { a = rand() % POP_SIZE; } while (a == i && ++intentos < max_intentos); // si 'a' generó un individuo y el ciclo no fue infinito
            intentos = 0; // reset
            do { b = rand() % POP_SIZE; } while (( b == i || b == a ) && ++intentos < max_intentos); 
            intentos = 0; // reset
            do { c = rand() % POP_SIZE; } while (( c == i || c == a || c == b ) && ++intentos < max_intentos);
            
            if ( intentos >= max_intentos ) { // Si alcanzó el máximo de intentos
                printf("ADVERTENCIA: No se pudieron seleccionar índices válidos\n");
                continue; // Decimos que el individuo no fue generado correctamente y, por ende, pasa al siguente
            }
        
            // Mutacion y recombinación (cruza)
            Individuo trial = poblacion[i];
            if ((rand() / (double)RAND_MAX) < CR) trial.iteraciones = (int)(poblacion[a].iteraciones + F * (poblacion[b].iteraciones - poblacion[c].iteraciones));
            if ((rand() / (double)RAND_MAX) < CR) trial.rho = poblacion[a].rho + F * (poblacion[b].rho - poblacion[c].rho);
            if ((rand() / (double)RAND_MAX) < CR) trial.alpha = poblacion[a].alpha + F * (poblacion[b].alpha - poblacion[c].alpha);
            if ((rand() / (double)RAND_MAX) < CR) trial.beta = poblacion[a].beta + F * (poblacion[b].beta - poblacion[c].beta);
            if ((rand() / (double)RAND_MAX) < CR) trial.num_hormigas = (int)(poblacion[a].num_hormigas + F * (poblacion[b].num_hormigas - poblacion[c].num_hormigas));
            
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
                trial.alpha = MIN_ALPHA;
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

            // Reemplazamos si el nuevo individuo es mejor
            double fitness_trial = evaluar(trial);
            printf("\n* Fitness Trial (primera medida de Robustez) = %lf\n", fitness_trial);
            double fitness_actual = evaluar(poblacion[i]);
            printf("* Fitness Actual (segunda medida de Robustez) = %lf\n", fitness_actual);

            // Si la emisión de la señal de interrupción resultó efectiva
            // con un único envío, esto implica que la ejecución del programa
            // finalizó de manera abrupta, pero satisfactoria, sin requerir señales
            // adicionales para su terminación
            if ( contador_ctrlC != 2 ) {
                // Compara si se obtiene alguna mejora (buena solución o mejores parámetros de control)
                if ( fitness_trial < fitness_actual ) {
                    poblacion[i] = trial;
                    if ( fitness_trial < mejor_fitness ) {
                        mejor_fitness = fitness_trial;
                        mejor = trial;
                    }
                }
                // Al finalizar el procesamiento del individuo... 
                // Actualizamos el número del indice (para continuar con el siguiente)
                poblacion_actual = i + 1; 
            } 
            // En caso contrario, la comparación no se efectuará,
            // ni se procederá a la siguiente población, dado que
            // la ejecución no concluyó de manera correcta (es decir, de forma completa)
           
            printf("\n---- Procesamiento de la población [%d] **completado exitosamente** ----\n\n", i + 1);

            // Si detecta la señal de interrupción en un número de población concreto
            // (es decir, si el usuario presionó CTRL + C antes de terminar cierta generación),
            // y además se interrumpió de manera satisfactoria y súbita (i.e. al instante)
            if ( detener_evolucionDiferencial ) {
                // Guarda automáticamente todo dato actual (con la población parcial procesada)
                checkpoint(poblacion, poblacion_actual, gen, mejor_fitness, mejor);
                printf("\t");
                system("pause");
                limpiar_pantalla();
                printf("\n\t**Interrupción detectada**. Punto de control **guardado** (Generación: %d, Población Procesada: %d). Procediendo con la terminación del programa...\n", gen + 1, poblacion_actual);
                sleep(5);
                limpiar_pantalla();
                free(poblacion); // Borra toda información anteriormente guardada en la población
                exit(0); // Y nos salimos (detenemos) de manera controlada del programa
            }
          
        } // Fin del ciclo for (INTERNO = Población)

        // De otro modo, se completó la generación específica, por tanto, resetea el índice de población
        poblacion_actual = 0; 
        printf("\n>>Proceso de generación [%d] **concluido satisfactoriamente**<<\n\n", gen + 1);

        // Para cada generación finalizada, realizamos el autoguardado de nuestros datos importantes
        checkpoint(poblacion, poblacion_actual, gen + 1, mejor_fitness, mejor);

    } // Fin del ciclo for (EXTERNO = Generación)

    free(poblacion); // Borramos nuevamente la memoria de la estructura de datos
    
    // Mostramos la mejor configuración encontrada
    printf("\nMejores parámetros:\n");
    printf("\t- Iteraciones: %d\n", mejor.iteraciones);
    printf("\t- Evaporación (rho): %lf\n", mejor.rho);
    printf("\t- Alpha: %lf\n", mejor.alpha);
    printf("\t- Beta: %lf\n", mejor.beta);
    printf("\t- Número de hormigas: %d\n\n", mejor.num_hormigas);

} // Fin de la función evolucion_diferencial

int main (void) {
    printf("\n**Observación:** Si requiere acceder a los parámetros de control en un momento posterior (por ejemplo, iniciar la"
           " ejecución hoy y, debido a su posible prolongación, completarla mañana),\npuede hacerlo con certeza enviando la señal"
           " de interrupción mediante la siguiente combinación de teclas: \"CTRL + C\". Realizando esto, se **guardará automáticamente** todo dato importante.\n");

    printf("\n\tADVERTENCIA: Si el **conjunto de nodos** y **enlaces** alcanza valores elevados, conformando una red de gran tamaño y complejidad,"
           " es posible que un único envío de la señal de interrupción\n\tmediante [CTRL + C] no sea suficiente para finalizar la ejecución del programa."
           " En tal caso, será necesario realizar un segundo envío.\n\tEn **resumen:** Presione 'CTRL + C' dos veces si la primera interrupción no logra detener el proceso.\n\n");

    system("pause");
    limpiar_pantalla();

    printf("\n\nIniciando optimización con Evolución Diferencial...\n");
    sleep(2);
    evolucion_diferencial();

    return 0;
}
