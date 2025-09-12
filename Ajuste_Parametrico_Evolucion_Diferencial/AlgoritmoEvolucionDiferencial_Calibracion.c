
/*
    -- Autor: Edwin Montes Orozco.
    -- Ajustes implementados por: Stephen Luna Ramírez.
    -- Fecha:  [8, 9] de septiembre de 2025.
    -- Asignatura: Proyecto Terminal (Tesis).
    -- Instructor: Edwin Montes Orozco.
    ========================================================================================================

    ### Diseño e Implementación del Algoritmo Evolución Diferencial

    Con la base conceptual y las herramientas necesarias para el análisis de robustez en redes complejas 
    mediante metaheurísticas (en particular, el **Ant System**), se desarrolla un módulo independiente que 
    implementa el algoritmo Evolución Diferencial (DE). El primer (como este segundo) módulo está disponible 
    en el repositorio (GitHub) y su propósito principal es **calibrar automáticamente** los parámetros de 
    control del Ant System para mejorar la calidad de las soluciones y reducir los tiempos de ejecución.

    **Evolución Diferencial** opera sobre una población de vectores candidatos que, mediante operadores 
    genéticos simples, buscan iterativamente mejorar la medida de calidad definida (función objetivo).

    El flujo funcional se articula en tres fases esenciales:

        (1) Recombinación (mutación y cruce).
        (2) Evaluación de la calidad de los candidatos.
        (3) Selección de la generación siguiente.

    En síntesis, DE proporciona una metodología de **optimización continua** para identificar parámetros de 
    control que aumenten la eficacia y la robustez del Ant System en la resolución del problema combinatorio 
    planteado.
*/

/*-------------------------------------------------------------------------------------
    Conjunto de bibliotecas estándar del lenguaje C empleadas en el desarrollo del
    algoritmo de **Evolución Diferencial**. 
    
    Estas bibliotecas proporcionan funciones auxiliares para la gestión de datos, la 
    realización de cálculos numéricos y otras operaciones fundamentales, garantizando 
    un manejo correcto de la información y facilitando la implementación eficiente del 
    programa.
*/

#include <stdio.h>   // Biblioteca estándar de C para operaciones de entrada y salida (E/S)
#include <stdlib.h>  // Biblioteca que ofrece funciones para la gestión de memoria dinámica (Heap), conversión numérica y utilidades generales, optimizando el manejo de datos y previniendo fugas de memoria
#include <stdbool.h> // En C, a diferencia de C++, no existe el tipo nativo 'bool'; esta biblioteca permite manejar valores booleanos (true/false) de forma estandarizada y eficiente
#include <time.h>    // Biblioteca estándar para la gestión de fechas y horas, que además permite formatear información temporal (en este caso, mediante la función 'time')

/*
    Configuración y datos iniciales para la ejecución del algoritmo de Evolución Diferencial,
    empleados en la optimización de los **parámetros de control del Ant System**.
 
    Este bloque especifica parámetros clave tales como el tamaño de la población, el número
    de generaciones (iteraciones), el factor de mutación y el factor de recombinación. Estos
    valores constituyen la base del proceso evolutivo que busca la calibración óptima de los
    parámetros de la colonia, con el fin de mejorar la calidad y la robustez de las soluciones.
 
    A continuación se especifican los parámetros:
*/

#define ESCALA_EVOLUTIVA_DE_LA_POBLACION    10  // Tamaño total de la población en Evolución Diferencial, es decir, el número de individuos candidatos a optimizar
#define MAX_GENERACIONES                    50  // Número máximo de generaciones (ciclos evolutivos permitidos)
#define PESO_DIFERENCIAL                    0.5 // Factor de Mutación
#define CR                                  0.5 // Factor de Recombinación

/*
    **Rangos predefinidos** de los parámetros de control del algoritmo Ant System 
    que serán optimizados mediante Evolución Diferencial:
*/

// Número de iteraciones (t), definido dentro del rango [1, 100]
#define MIN_ITERACIONES 1
#define MAX_ITERACIONES 100
// Parámetro Rho (tasa de evaporación de feromona) con un rango permitido de [0.01, 0.99]
#define MIN_RHO         0.01
#define MAX_RHO         0.99
// Parámetro Alpha (influencia de la feromona) con rango permitido de [0.1, 5.0]
#define MIN_ALPHA       0.1
#define MAX_ALPHA       5.0
// Parámetro Beta (influencia heurística) con rango permitido de [0.1, 5.0]
#define MIN_BETA        0.1
#define MAX_BETA        5.0
// Número de hormigas o agentes heurísticos (m), definido dentro del rango [1, 100]
#define MIN_HORMIGAS    1
#define MAX_HORMIGAS    100

// Conjunto de parámetros de cada individuo, esenciales para Ant System y usados 
// exclusivamente para hallar el costo mínimo de la función objetivo (mínima 
// cantidad de nodos en la red)
typedef struct {
    int     iteraciones;
    double  rho;
    double  alpha;
    double  beta;
    int     hormigas;
} Individuo;

/*
    Variables globales cuya información se actualiza continuamente en Evolución Diferencial 
    para su uso en funciones, cálculos, decisiones y almacenamiento de datos:
*/

Individuo *poblacion;
Individuo  candidato_idoneo;   
int        poblacion_actual = 0;
double     mejor_fitness    = 1e9;

// ----  Prototipos de funciones que definen la arquitectura general y el diseño abstracto del programa  ----

void    evolucionDiferencial(void);
double  configurar_dominio_aleatorio(double minimo, double maximo);
bool    restaurar_estado_de_evolucion(Individuo *poblacion, int *p_actual, int *generacion_actual, double *mejor_fitness, Individuo *mejor_individuo);
void    gestionar_estado_de_evolucion(Individuo poblacion[], int poblacion_actual, int generacion_actual, double mejor_fitness, Individuo mejor_individuo);
double  evaluar_individuo(Individuo individuo);

int main (void) { // Now... Let's try this ON! d:
    printf("\n---Iniciando Optimización con Evolución Diferencial para ANT SYSTEM--->\n\n");
    evolucionDiferencial();
    return 0;
}

/* ====================================================================================
    
    Implementación de funciones que representan el comportamiento global del sistema,
    actuando sobre los datos del algoritmo de Evolución Diferencial. 
    
    Su objetivo es **calibrar** y **optimizar** los parámetros de control del algoritmo 
    metaheurístico Ant System, permitiendo obtener soluciones más robustas y eficientes 
    en el proceso de optimización.
*/

/*
    Determina los parámetros óptimos del algoritmo Ant System con el fin de maximizar
    la eficiencia en la optimización de la función objetivo (ya sea maximizada o minimizada).

    Se utiliza el enfoque de Evolución Diferencial, adaptado a la naturaleza discreta
    del problema, permitiendo aprovechar un método tradicionalmente continuo en un
    contexto de **optimización combinatoria**
*/

void evolucionDiferencial(void) {
    // Inicializa y reserva memoria para la estructura que contiene la población de parámetros de control del Ant System
    poblacion = malloc (ESCALA_EVOLUTIVA_DE_LA_POBLACION * sizeof(Individuo));
    // Inicializa el generador de números aleatorios con una semilla, asegurando valores positivos para evitar parámetros o rangos negativos
    srand((unsigned)time(NULL));

    int generacion_actual = 0; // Contador de la generación actual a evaluar y calibrar, inicializado en cero

    // Verifica si existe un proceso reciente en el archivo de salida (autoguardado)
    if ( restaurar_estado_de_evolucion(poblacion, &poblacion_actual, &generacion_actual, &mejor_fitness, &candidato_idoneo) ) {
        printf("\n**Restauración de Datos Exitoso**.\n\nReanudando desde generación [ %d ], población [ %d ].\n\n", generacion_actual, poblacion_actual);
    }
    else { // De lo contrario, **inicializa** la población desde cero
        for ( int p = 0; p < ESCALA_EVOLUTIVA_DE_LA_POBLACION; ++p ) {
            /*
                Inicializa de manera aleatoria los parámetros de control de cada individuo
                en la población. Este procedimiento establece heurísticamente los rangos
                válidos para cada parámetro, garantizando diversidad inicial y cobertura
                del espacio de búsqueda en el algoritmo evolutivo
            */
            poblacion[p].iteraciones = (int)configurar_dominio_aleatorio(MIN_ITERACIONES, MAX_ITERACIONES);
            poblacion[p].rho         = configurar_dominio_aleatorio(MIN_RHO, MAX_RHO);
            poblacion[p].alpha       = configurar_dominio_aleatorio(MIN_ALPHA, MAX_ALPHA);
            poblacion[p].beta        = configurar_dominio_aleatorio(MIN_BETA, MAX_BETA);
            poblacion[p].hormigas    = (int)configurar_dominio_aleatorio(MIN_HORMIGAS, MAX_HORMIGAS);
        }        
    }

   // Según el número de generaciones, ejecuta las llamadas a la función objetivo
    for ( int gen = generacion_actual; gen < MAX_GENERACIONES; ++gen ) {
        printf("\nGeneración: %d\n", gen + 1);
        // En cada iteración evolutiva, se evalúa la población, conservando individuos
        // mejorados y descartando los que no presentan mejoras
        for ( int p = poblacion_actual; p < ESCALA_EVOLUTIVA_DE_LA_POBLACION; ++p ) {
            printf("  Población: %d\n\n", p + 1);

            // Guarda continuamente el estado de evolución del individuo (los parámetros de control)
            gestionar_estado_de_evolucion(poblacion, poblacion_actual, gen, mejor_fitness, candidato_idoneo);

           // Selecciona aleatoriamente tres individuos distintos (por sus índices) de la población
            int a, b, c;

            do { a = rand() % ESCALA_EVOLUTIVA_DE_LA_POBLACION; } while( a == p );
            do { b = rand() % ESCALA_EVOLUTIVA_DE_LA_POBLACION; } while( b == p || b == a );
            do { c = rand() % ESCALA_EVOLUTIVA_DE_LA_POBLACION; } while (c == p || c == a || c == b);

            // Comienza la evolución del individuo: Mutación y Recombinación
            Individuo trial = poblacion[p];
            if ( ( rand() / (double)RAND_MAX ) < CR ) trial.iteraciones = (int)(poblacion[a].iteraciones + PESO_DIFERENCIAL * (poblacion[b].iteraciones - poblacion[c].iteraciones) );
            if ( ( rand() / (double)RAND_MAX ) < CR ) trial.rho         = poblacion[a].rho               + PESO_DIFERENCIAL * ( poblacion[b].rho        - poblacion[c].rho          );
            if ( ( rand() / (double)RAND_MAX ) < CR ) trial.alpha       = poblacion[a].alpha             + PESO_DIFERENCIAL * ( poblacion[b].alpha      - poblacion[c].alpha        );
            if ( ( rand() / (double)RAND_MAX ) < CR ) trial.beta        = poblacion[a].beta              + PESO_DIFERENCIAL * ( poblacion[b].beta       - poblacion[c].beta         );
            if ( ( rand() / (double)RAND_MAX ) < CR ) trial.hormigas    = (int)poblacion[a].hormigas     + PESO_DIFERENCIAL * ( poblacion[b].hormigas   - poblacion[c].hormigas     );

            /*
                Implementa **restricciones específicas** para prevenir que ciertos valores alcancen niveles
                excesivos que puedan provocar ciclos internos inestables o descontrolados en el algoritmo.

                En particular, se aplican límites a los niveles de feromona que podrían generar
                inconsistencias o incompatibilidades en la construcción de soluciones.

                Cada restricción define un rango o métrica válida, asegurando estabilidad y coherencia
                en el proceso de optimización.
            */

            if ( trial.iteraciones < MIN_ITERACIONES )     trial.iteraciones = MIN_ITERACIONES;
            else
                if ( trial.iteraciones > MAX_ITERACIONES ) trial.iteraciones = MAX_ITERACIONES - (trial.iteraciones - MAX_ITERACIONES);
            
            if ( trial.rho < MIN_RHO )    trial.rho = MIN_RHO;
            else
                if (trial.rho > MAX_RHO ) trial.rho = MAX_RHO - (trial.rho - MAX_RHO);

            if ( trial.alpha < MIN_ALPHA )     trial.alpha = MIN_ALPHA;
            else
                if ( trial.alpha > MAX_ALPHA ) trial.alpha = MAX_ALPHA - (trial.alpha - MAX_ALPHA);

            if ( trial.beta < MIN_BETA )     trial.beta = MIN_BETA;
            else
                if ( trial.beta > MAX_BETA ) trial.beta = MAX_BETA - (trial.beta - MAX_BETA);
            
            if ( trial.hormigas < MIN_HORMIGAS )     trial.hormigas = MIN_HORMIGAS;
            else
                if ( trial.hormigas > MAX_HORMIGAS ) trial.hormigas = MAX_HORMIGAS - (trial.hormigas - MAX_HORMIGAS);

            // == Inicia la evaluación para reemplazar un individuo si la nueva solución es mejor ==
            double fitness_trial  = evaluar_individuo(trial);
            double fitness_actual = evaluar_individuo(poblacion[p]);

            if ( fitness_trial < fitness_actual ) {
                poblacion[p] = trial;
                if ( fitness_trial < mejor_fitness ) {
                    mejor_fitness = fitness_trial;
                    candidato_idoneo = trial;
                }
            }

            poblacion_actual = p + 1; // Actualiza el número de índice (población) para continuar con el siguiente individuo
            printf("    Fitness trial: %.6lf | Fitness actual: %.6lf\n", fitness_trial, fitness_actual);

        } // Fin bucle for (INTERNO): Iteración de Población (Individuos mejores o peores)

        poblacion_actual = 0; // Fin de la generación: se utiliza 'reset' para reiniciar las poblaciones
        printf("\n>> Generación %d Completada <<\n", gen + 1);

        // Realiza el autoguardado tras finalizar una determinada generación
        gestionar_estado_de_evolucion(poblacion, poblacion_actual, gen + 1, mejor_fitness, candidato_idoneo);

    } // Fin bucle for (EXTERNO): Ciclos Evolutivos
    
    free(poblacion); // Borra dinámicamente los datos en memoria

    // Muestra la mejor configuración de parámetros encontrada
    printf("\nMejores parámetros encontrados:\n\n");
    printf("\t+ Iteraciones: %d\n",    candidato_idoneo.iteraciones);
    printf("\t+ Rho:         %.6lf\n", candidato_idoneo.rho);
    printf("\t+ Alpha:       %.6lf\n", candidato_idoneo.alpha);
    printf("\t+ Beta:        %.6lf\n", candidato_idoneo.beta);
    printf("\t+ Hormigas:    %d\n",    candidato_idoneo.hormigas);

} // Fin de la función evolucionDiferencial

/*
    Genera un número aleatorio comprendido entre los valores mínimo y máximo
    establecidos para cada parámetro de control del Ant System. Esta función
    asegura que la inicialización de los parámetros respete los límites definidos,
    contribuyendo a la diversidad y cobertura del espacio de búsqueda
*/

double configurar_dominio_aleatorio(double minimo, double maximo) {
    // Calcula una sola vez el inverso de RAND_MAX para optimizar la generación 
    // de números aleatorios, evitando divisiones repetitivas
    static const double invertir_rand_max = 1.0 / (double) RAND_MAX;

    // Finalmente, regresa el rango deseado del parámetro de control especificado
    return ( minimo + ( (double)rand() * invertir_rand_max ) * ( maximo - minimo ) );
} // Fin de la función configurar_dominio_aleatorio

/*
    Permite reanudar la ejecución del programa conservando la información previamente
    obtenida. Esta funcionalidad asegura la recuperación de los datos de cada individuo,
    considerando el número de generaciones y la población establecida, y facilita
    la optimización continua de los parámetros de control del algoritmo metaheurístico
    Ant System
*/

bool restaurar_estado_de_evolucion(Individuo *poblacion, int *p_actual, int *generacion_actual, 
                                   double *mejor_fitness, Individuo *mejor_individuo) {
    FILE *ED_archivo_de_salida = fopen("Historial_Evolutivo_ED/ejecutar_datos_de_reanudacion.txt", "r");
    
    // Verifica si en el archivo de autoguardado existen datos
    // para reanudarlo y continuar con la calibración de parámetros
    if ( !ED_archivo_de_salida ) return false;
    
    // Comprueba y lee primera línea: generación, población actual, mejor fitness
    if ( fscanf(ED_archivo_de_salida, "%d %d %lf", generacion_actual, p_actual, mejor_fitness) != 3 ) {
        fclose(ED_archivo_de_salida);
        return false;
    }

    // Comprueba y lee el mejor individuo (mejores parámetros de control)
    if ( fscanf(ED_archivo_de_salida, "%d %lf %lf %lf %d", &mejor_individuo->iteraciones, &mejor_individuo->rho, 
                &mejor_individuo->alpha, &mejor_individuo->beta, &mejor_individuo->hormigas) != 5 ) {
        fclose(ED_archivo_de_salida);
        return false;
    }

    // Finalmente, lee toda la población realizada hasta el momento
    for ( int p = 0; p < ESCALA_EVOLUTIVA_DE_LA_POBLACION; ++p ) {
        if ( fscanf(ED_archivo_de_salida, "%d %lf %lf %lf %d", &poblacion[p].iteraciones, 
            &poblacion[p].rho, &poblacion[p].alpha, &poblacion[p].beta, &poblacion[p].hormigas) != 5 ) {
            fclose(ED_archivo_de_salida);
            return false;
        }
    }
    fclose(ED_archivo_de_salida);

    return true;
} // Fin de la función restaurar_estado_de_evolucion

// Permite la preservación de datos del 'individuo', evitando la pérdida de información en caso
// de suspensión o interrupción. Permite al usuario finalizar la ejecución del programa y reiniciar
// posteriormente sin necesidad de repetir el proceso de inicialización.
// Diseñada para manejar eficientemente grandes volúmenes de datos asociados al conjunto de nodos y enlaces
void gestionar_estado_de_evolucion(Individuo poblacion[], int poblacion_actual, int generacion_actual, 
                                   double mejor_fitness, Individuo mejor_individuo) {
    FILE *ED_archivo_de_entrada = fopen("Historial_Evolutivo_ED/ejecutar_datos_de_reanudacion.txt", "w");
    
    // Comprueba si es posible escribir (guardar) los datos
    // en el archivo correspondiente para continuar con la optimización
    // de parámetros más adelante
    if ( !ED_archivo_de_entrada ) {
        printf("\nSe ha detectado un error en la apertura y escritura del archivo \"ejecutar_datos_de_reanudacion.txt\",\n"
               "impidiendo el almacenamiento de información relevante. Por favor de verificar e intentar nuevamente.\n");
        return;
    }

    /*
        Para minimizar las múltiples llamadas a fprintf(), que implican parsing de formato
        y escritura independiente, se emplea un único buffer grande y fwrite() para escribir
        toda la información de una sola vez. Este enfoque, similar al usado en la implementación
        de Ant System en C++, incrementa la eficiencia y reduce el tiempo de ejecución.
 
        Además, se utiliza el especificador '%.17g' en lugar de %lf para garantizar máxima
        precisión en valores de tipo double, evitando residuos o "basura" numérica en la salida.
    */

    char buffer[ESCALA_EVOLUTIVA_DE_LA_POBLACION * 130]; // Buffer temporal que contendrá todos los datos necesarios de la optimización de calibración con un tamaño estimado
    char *escritura = buffer; // Asignamos memoria dinámica del buffer para concatenar varias líneas en memoria antes de volcarlas

    // Escribe el encabezado como primero todo
    escritura += sprintf(escritura, "%d %d %.17g\n", generacion_actual, poblacion_actual, mejor_fitness);
    // Guarda al mejor individuo hasta ese momento
    escritura += sprintf(escritura, "%d %.17g %.17g %.17g %d\n", 
                         mejor_individuo.iteraciones, mejor_individuo.rho, mejor_individuo.alpha, mejor_individuo.beta, mejor_individuo.hormigas);
    
    // Guarda la población completa
    for ( int p = 0; p < ESCALA_EVOLUTIVA_DE_LA_POBLACION; ++p ) {
        escritura += sprintf(escritura, "%d %.17g %.17g %.17g %d\n",
                            poblacion[p].iteraciones, poblacion[p].rho, poblacion[p].alpha, poblacion[p].beta, poblacion[p].hormigas);
    }

    // Finalmente, escribimos todo en un solo paso
    fwrite(buffer, 1, escritura - buffer, ED_archivo_de_entrada);
    // Cierra el archivo para evitar perdida de datos
    fclose(ED_archivo_de_entrada);
} // Fin de la función gestionar_estado_de_evolucion

// Evalúa la robustez ejecutando el programa (escrito en Lenguaje C++) con los parámetros de control optimizados 
// del Ant System mediante Evolución Diferencial
double evaluar_individuo(Individuo individuo) {
    // Crea un archivo marcador para que **Ant System** pueda detectar que la ejecución de Evolución Diferencial
    // ha sido inicializado o ejecutado
    FILE *archivo_temporal = fopen("indicador_de_progreso_ED.tmp", "w");
    if ( archivo_temporal ) fclose(archivo_temporal);

    // Construcción del comando, según el sistema operativo del usuario
    char comando[550];

    /*
        Carga las instancias del problema presentes en el directorio especificado ("Casos_de_Estudio").
        En este contexto se contempla un archivo de ejemplo para pruebas ('prueba.txt').
 
        El último parámetro de entrada indica la representación de grafo a emplear:

          - 0 : Matriz de adyacencia
          - 1 : Lista  de adyacencia
 
        RECOMENDACIÓN: Para redes con grados elevados o estructuras dispersas, es preferible
        utilizar **listas de adyacencia** por su eficiencia espacial y operativa en recorridos y
        actualizaciones, frente a la representación matricial que incurre en mayor coste
        cuando la red es esparsa.
    */

    #ifdef _WIN32 // Windows 
        snprintf(comando, sizeof(comando), ".\\analizador_de_ROBUSTEZ_de_redes.exe %d %lf %lf %lf %d %s %d", 
                individuo.iteraciones, individuo.rho, individuo.alpha, individuo.beta, individuo.hormigas, "Casos_de_Estudio/prueba.txt", 1);
    #else        // Sistemas UNIX
         snprintf(comando, sizeof(comando), "./analizador_de_ROBUSTEZ_de_redes %d %lf %lf %lf %d %s %d", 
                individuo.iteraciones, individuo.rho, individuo.alpha, individuo.beta, individuo.hormigas, "Casos_de_Estudio/prueba.txt", 1);
    #endif

    /*
        Lamentablemente, la funcionalidad de 'system()' es un intérprete de comandos que parsea la cadena y crea un nuevo proceso,
        resultando muy costoso en términos computacionales. No obstante, en crear y cerrar archivo de entrada, también resulta
        disminución de tiempo; por tanto, se utilizara 'popen()' para ejecutar el programa y leer su salida directamente en memoria, sin 
        necesidad de escribir el archivo "resultado.txt" que solía tenerse en el programa original. 

    */
    
    FILE *fp = popen(comando, "r");
    // Si existe posible fallo al ejecutar
    if ( !fp ) return 1e9; // Devuelve un valor grande (mala solución)
    // De lo contrario
    double robustez;  // Define el valor o métrica robustez deseada (inicialmente vacía)

    // Comprueba si la métrica de robustez es existente
    if ( fscanf(fp, "%lf", &robustez) != 1 ) {
        pclose(fp);
        return 1e9; // Si no fue así, indica que existió un fallo al leer
    }
    pclose(fp);

    return robustez;
} // Fin de la función evaluar_individuo

