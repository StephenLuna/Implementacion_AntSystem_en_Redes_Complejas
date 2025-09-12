# **Robustez** de Redes Complejas a través de la Inteligencia de Enjambre: Aplicación del Método **Ant System**

El presente documento no busca profundizar en la definición de los conceptos que constituyen el sistema, al estilo de un tratado científico o un ensayo filosófico. En cambio, se ofrece una descripción concisa, similar a un mapa que indica las rutas y métodos para transitar de un origen a un destino (identificados a través de colores, símbolos, etc.). De igual manera, se presenta una guía operativa que detalla el correcto uso del progama: especifica cómo **compilarlo** y **ejecutarlo**, y cómo aplicar las técnicas heurísticas–específicamente, el **Sistema Hormiga** y la **Evolución Diferencial**–sobre una representación de la red.
Esta red modela el problema de optimización (ya sea mediante una fase de **minimización** o **maximización**) de los nodos, dividiéndola en componentes centrales: un conjunto separador, que es el objetivo principal de la solución, y dos componentes mayores, designados como A y B, cuya inclusión resulta indeseable por los elevados costos que implican (o, en el caso de la *maximización*, actúan como restricciones, denominadas también **penalizaciones**).

La **red original** se puede representar mediante dos alternativas, seleccionables por el usuario (ya sea en el programa principal o en el módulo secundario de Evolución Diferencial, un algoritmo heurístico que requiere modificar únicamente un parámetro manualmente): la **matriz de adyacencia** y la **lista de adyacencia**. Ambas opciones modelan el mismo grafo (definido en términos de enlaces y vértices) y tienen como objetivo descomponer la red en los subgrafos descritos previamente para evaluar una función de costo.

En consecuencia, aunque ambas representaciones conducen al mismo resultado, se diferencian en su interpretación y en la eficiencia para identificar los nodos a desconectar, dependiendo de las características de la estructura de datos en términos de **tiempo** y **espacio**. Por ejemplo, en conjuntos extensos de datos, la matriz de adyacencia podría resultar menos eficiente que la lista, que, por otro lado, puede ofrecer una búsqueda y almacenamiento más rápidos. No obstante, el análisis comparativo de la *eficiencia computacional*–medido, por ejemplo, en el **peor caso** (O(grande))–queda fuera del alcance del presente estudio, aunque se persiga optimizar los tiempos de ejecución computacional. 

---
---

**Nota de actualización:**

En la tesis se detallan rigurosamente las implicaciones de la complejidad temporal, directamente vinculadas al diseño e implementación del metaheurístico **Ant System**. En este documento se exponen únicamente los aspectos críticos que han impulsado la presente actualización del proyecto, con especial énfasis en optimizar la eficiencia y reducir los tiempos de análisis en redes de gran escala.

---
---

En el presente repositorio se disponen de carpetas que contienen los programas auxiliares necesarios para generar una red específica y realizar su análisis de **robustez**. Dentro de la carpeta denominada *Casos_de_Estudio* se encuentran los dos conjuntos principales de datos utilizados durante el proyecto (**Facebook** y **Twitter**), así como dos archivos adicionales denominados **grafo_prueba.txt** y **prueba.txt**. Este último no forma parte de las instancias originales, sino que se ofrece como modelo de ejemplo para que el usuario visualice rápidamente los resultados sin tener que esperar a obtener una solución completa, lo que optimiza el uso de recursos computacionales, como **memoria** y **procesamiento**.

Al ejecutar el analizador con las instancias **grafo_prueba.txt** (distribución equilibrada de nodos) o **prueba.txt** (distribución irregular utilizada para diagnosticar las causas de la imposibilidad de analizar la instancia ‘Twitter’), el usuario debe especificar los parámetros de **Evolución Diferencial** en el siguiente orden:

* **Número de iteraciones** (%d, entero positivo)
* **Rho** (%lf, número con/sin decimales; representa el *coeficiente de evaporación*)
* **Alpha** (%lf, representando la *influencia de feromonas*)
* **Beta** (%lf; constituye la *influencia heurística*)
* **Número de hormigas** (%d)
* **Nombre del archivo** (%s; incluir la ruta y la extensión (si aplica), por ejemplo, ".txt")
* **Opción de grafo** (%d; (0) para **matriz de adyacencia** y (1) para la **lista de adyacencia**)

---


**Primera Observación:** Para ejecutar correctamente estos programas auxiliares, se recomienda extraerlos sin conservar la carpeta contenedora original, de manera que todos los archivos queden en un único directorio de ejecución.

---

**Segunda Observación:** Debido a las mejoras introducidas en el diseño algorítmico, todo el código fuente (por ejemplo, los módulos de **Evolución Diferencial** y **Ant System**) junto con sus correspondientes archivos de cabecera (.h) deben agruparse en un **único directorio raíz**. En dicho directorio se mantendrán inalterables **tres subcarpetas** fundamentales:

1. Casos_de_Estudio
2. Historial_Evolutivo_ED
3. Resultados_Empiricos_Optimizacion

Bajo ninguna circunstancia deben **eliminarse**, **renombrarse** o **trasladarse** los archivos contenidos en estas carpetas, salvo la adición de nuevos casos de estudio en “Casos_de_Estudio”.

Al ejecutar la aplicación —seleccionando previamente el algoritmo de Evolución Diferencial o Ant System— el sistema generará automáticamente:

- En **Historial_Evolutivo_ED**, un fichero de bitácora que permite reanudar el proceso evolutivo sin pérdida de información, siempre que se haya completado al menos una generación y su población asociada.

- En **Resultados_Empíricos_Optimizacion**, los reportes de validación y verificación de los resultados obtenidos por el metaheurístico Ant System.

Esta organización garantiza la trazabilidad del flujo de datos y facilita la reproducibilidad de los experimentos.

---

Si se desea ejecutar únicamente el **programa principal** (es decir, la aplicación del Sistema Hormiga), el usuario debe ingresar **manualmente** los parámetros requeridos–respetando el orden establecido–en la terminal de su computadora, sobre la cual se procederá a compilar y ejecutar el programa. En caso de que no se proporcione la cantidad exacta de argumentos o se presione simplemente la tecla **ENTER**, la consola indicará las instrucciones pertinentes; sin embargo, el programa finalizará de inmediato al no contar con una entrada válida que permita iniciar su ejecución y generar los resultados esperados.

**Nota:** Se recomienda leer el documento original (tesis) antes de proceder, pues en él se abordan conceptos y características fundamentales relacionados con las redes y los procesos heurísticos, así como la relevancia de la optimización–analizada en dos contextos: **continua** y **combinatoria**–siendo este último el enfoque principal del estudio.

Con el fin de ofrecer directrices precisas, a continuación se resumen de manera concisa los pasos necesarios para el correcto uso del sistema implementado:

En primer lugar, es importante destacar que el proyecto integra dos enfoques computacionales destinados a resolver problemas de optimización en grafos, particularmente en el contexto de redes:

**1.** **Método de Hormigas**

Un algoritmo inspirado en el comportamiento colectivo de las hormigas que, mediante la simulación de procesos de exploración y explotación, **busca** construir soluciones óptimas o cercanas a óptimas. Cada **hormiga** recorre el grafo (red), evita caminos previamente visitados (a través de una **lista tabú**), recalcula probabilidades dinámicamente y determina su contribución a la función objetivo. La **actualización de feromonas** se realiza de forma iterativa, permitiendo que las soluciones mejores se refuercen conforme se explora el espacio de soluciones.

**2.** **Algoritmo Evolutivo (Evolución Diferencial - DE)**

Incorporado en un programa desarrollado en C, este algoritmo calibra de forma automática los **parámetros de control** (como el **número de iteraciones**, **Rho**, **Alpha**, **Beta** y el **número de hormigas**) utilizados por el algoritmo de hormigas. De esta forma se garantiza que el sistema se adapte a las condiciones óptimas para la búsqueda, logrando una robustez adicional en los resultados.

*Esta integración flexibiliza el proceso de **optimización**, permitiendo tanto la ejecución manual–donde se especifican los parámetros desde la terminal–como una ejecución semiautomática mediante el algoritmo evolutivo, lo que facilita la calibración de los parámetros sin intervención directa del usuario.*

## Herramientas y Lenguajes Utilizados

- **C++:** Implementación principal del sistema de hormigas, definición de estructuras, número de grado de cada nodo, cálculo de probabilidades, actualización de feromonas, evaluación de la función objetivo, entre otros.

- **C:** Implementación del algoritmo evolutivo encargado de **calibrar** los parámetros de control.

- **Compiladores:**

    * Para **C++**: Se utiliza `g++`.
    * Para **C**: Se utiliza `gcc`.

## Especificaciones Técnicas para la Ejecución del Programa tras la Actualización de Ant System

*Tras culminar el diseño y desarrollo algorítmico del proyecto —cuyo objetivo es evaluar la robustez de redes complejas mediante enfoques matemáticos y computacionales, como la **teoría de percolación inversa**, la **teoría de grafos** (esencial para entender el fenómeno del **componente gigante**) y el **problema del separador de vértices**—, y tras aplicar técnicas heurísticas para la implementación íntegra del Ant System, que planteó desafíos significativos de cómputo en términos de **costo** y **complejidad espacio-temporal**, consideré que el sistema estaba plenamente concluido.*

*En ese momento, estimé innecesario incorporar nuevos conocimientos o realizar ajustes adicionales al desarrollo algorítmico. Desde que inicié este proyecto —con más de un año de desarrollo— no he podido disponer de un descanso reparador ni de momentos de lectura o de escritura creativa.*

*He afrontado importantes desafíos para obtener resultados eficientes y de rápida ejecución. La programación, en este contexto, trasciende la mera implementación de soluciones: requiere un análisis minucioso de cada componente algorítmico para identificar oportunidades de reducción en el número de operaciones y en la complejidad temporal (**T(n)**). Esta rigurosidad, propia de todo problema de optimización, obliga a **maximizar** o **minimizar** la función objetivo según las restricciones establecidas y los parámetros de control del Ant System, y ha constituido el principal obstáculo para abordar redes de mayor tamaño o cardinalidad.* 

*El **programa inicial** no adolecía de fallos conceptuales graves; sin embargo, las pruebas de rendimiento —particularmente los tiempos de ejecución— y las limitadas simulaciones computacionales revelaron que su desempeño no satisfacía los criterios deseados. Durante su desarrollo se pasó por alto un proceso sistemático de optimización de los algoritmos encargados de generar el código y no se definieron claramente los casos límite que pudieran comprometer la correcta finalización de cada iteración.*

*En concreto, no se detectaron situaciones como el bloqueo de la lista tabú en ciclos infinitos. Este comportamiento subóptimo se originó en la falta de un análisis exhaustivo de los escenarios críticos susceptibles de simplificación y mejora, tanto en la estructura de datos como en la lógica de control del algoritmo.*

*En la versión inicial, el programa no segmentaba de manera óptima los objetivos esenciales del Ant System, lo que, sumado a una arquitectura subóptima, derivaba en un tiempo de cómputo de hasta **quince días** para obtener una solución óptima.*

*Con la implementación de un diseño modular, la redefinición de las tareas clave y la optimización heurística de sus componentes, el tiempo de ejecución se ha reducido drásticamente. Esto permite generar soluciones en plazos mínimos y evaluar la robustez de redes de gran escala, con un elevado número de nodos y conexiones.*

---

En esta **nueva implementación**, el sistema se organiza en **cinco programas independientes** además del **ejecutable principal**, todos ellos integrados bajo la estructura del metaheurístico Ant System. Para simplificar la compilación y prueba de distintas redes, se ha desarrollado un **Makefile** que automatiza el enlace de las cabeceras y fuentes de **Ant System** y **Evolución Diferencial**, asumiendo que todos los módulos residen en la raíz del proyecto.

Mediante este **Makefile**, el usuario puede ejecutar el sistema de **dos maneras**:

1. **Calibración automática** de los parámetros de control utilizando un algoritmo de Evolución Diferencial.

2. **Configuración manual** desde línea de comandos, especificando los parámetros de control, la instancia de entrada (carpeta **Casos_de_Estudio**) y el modelo de red (dos representaciones disponibles) para el análisis de robustez.

Al teclear en la terminal:

```bash
make all
```
Se compilarán y enlazarán los módulos del proyecto, generando **dos ejecutables**:

1. **calibrar_parametros**, encargado de la calibración automática de los parámetros de control.
2. **analizador_de_ROBUSTEZ_de_redes**, que pone en marcha el Ant System para el análisis de robustez.

Cada ejecutable puede invocarse de forma independiente, permitiendo al usuario seleccionar el mecanismo deseado para evaluar la robustez de redes complejas.

Para **eliminar** los binarios generados, basta con ejecutar:

```bash
make clean
```
El proceso de ejecución sigue el esquema habitual de versiones previas y varía en función del sistema operativo.

A continuación, se describen las **dos modalidades disponibles** para invocar el programa:


### 1. Ejecución Manual

Para ejecutar el **analizador de robustez**, invocar el ejecutable **analizador_de_ROBUSTEZ_de_redes** con los argumentos obligatorios:

**Windows**

```bash
.\analizador_de_ROBUSTEZ_de_redes.exe 3 0.5 1.2 1.0 2 Casos_de_Estudio/prueba.txt 1 
```

**Sistemas UNIX**

```bash
./analizador_de_ROBUSTEZ_de_redes 3 0.5 1.2 1.0 2 Casos_de_Estudio/prueba.txt 1 
```
Los **parámetros**, en el orden indicado, son:

* **Número de Iteraciones:** Cantidad de repeticiones del proceso de solución (ejemplo: `3`).

* **Rho:** Factor de evaporación de la feromona (ejemplo: `0.5`).

* **Alpha:** Peso asignado a la influencia de la feromona en el cálculo de probabilidades (ejemplo: `1.2`).

* **Beta:** Peso asignado al valor heurístico (ejemplo: `1.0`).

* **Número de Hormigas:** Cantidad de agentes que recorrerán el grafo, encontrando una solución posiblemente óptima (ejemplo: `2`).

* **Nombre del Archivo:** Archivo que contiene el grafo (por ejemplo, `Casos_de_Estudio/prueba.txt`).

* **Modelo de Red (Opción):** `0` para que el archivo se interprete como **matriz** y `1` para **lista**.

**Nota:** Si la invocación no incluye todos los parámetros requeridos, o éstos no cumplen con el tipo esperado, el sistema mostrará mensajes indicando el uso correcto de los parámetros, limpiará la pantalla y terminará la ejecución para evitar errores.

### 2. Ejecución Automática mediante Evolución Diferencial

Cuando se prefiera automatizar la calibración de parámetros, utilice el algoritmo de **Evolución Diferencial**. El procedimiento consta de **dos etapas**:

1. **Invocación de la regla del Makefile**

En la raíz del proyecto, ejecute:

```bash
make run_ED
```

Esta instrucción compila el módulo de calibración y genera el ejecutable **calibrar_parametros**.

2. **Ejecución del binario _calibrar_parametros_**

El programa resultante no requiere argumentos adicionales más allá de la ruta al archivo de instancia que describe la red:

**Windows**

```bash
.\calibrar_parametros.exe
```

**Sistemas UNIX**

```bash
./calibrar_parametros
```

La especificación del **formato de entrada** y las **opciones disponibles** para la red se detallan en la sección siguiente.

### 3. Especificación de Archivo de Entrada e Instancia

En el módulo de calibración (**AlgoritmoEvolucionDiferencial_Calibracion.c**), la función `evaluar_individuo()` emplea `snprintf()` para construir el comando de invocación del analizador de robustez:

**Windows**

> `snprintf(comando, sizeof(comando), ".\\analizador_de_ROBUSTEZ_de_redes.exe %d %lf %lf %lf %d %s %d", 
                individuo.iteraciones, individuo.rho, individuo.alpha, individuo.beta, individuo.hormigas, "Casos_de_Estudio/prueba.txt", 1);`

**Sistemas UNIX**

> `snprintf(comando, sizeof(comando), "./analizador_de_ROBUSTEZ_de_redes %d %lf %lf %lf %d %s %d", 
                individuo.iteraciones, individuo.rho, individuo.alpha, individuo.beta, individuo.hormigas, "Casos_de_Estudio/prueba.txt", 1);`


**IMPORTANTE:**

- El usuario solo debe especificar el **nombre del archivo de entrada** (instancia) y la **opción de modelo de grafo**.

- El resto de los argumentos se **generan automáticamente** a partir de los parámetros del individuo.

- El **historial de las generaciones individuales** se almacena en la carpeta **Historial_Evolutivo_ED**.

## Entradas Válidas y No Válidas

* **Entradas Válidas:**

    - **Números:** Los argumentos numéricos (**iteraciones**, **Rho**, **Alpha**, **Beta** y **número de hormigas**) deben ser del tipo adecuado (por ejemplo, **enteros** o **flotantes** según se requiera).

    - **Archivo:** El nombre del archivo debe corresponder a un fichero existente con el formato correcto (por ejemplo, "Casos_de_Estudio/grafo_prueba.txt").

    - **Modelo de Adyacencia (Red):** Sólo se aceptan `0` (para el grafo en forma de matriz) o `1` (para el grafo en forma de lista).

* **Entradas No Válidas:**

    - **Parámetros Incorrectos o Faltantes:** La omisión de algún argumento o proporcionar datos de un tipo inadecuado (por ejemplo, texto en lugar de un número o números negativos).

    - **Archivo Inválido:** Un archivo inexistente o con un formato que no se pueda intepretar correctamente.

    - **Opción Incorrecta:** Cualquier valor distinto a `0` o `1` para la opción del grafo (red, formalmente).

*El sistema **valida** los argumentos al **inicio de la ejecución**; en caso de detectar inconsistencias, notificará al usuario con un mensaje descriptivo y finalizará la ejecución de manera segura, evitando posibles errores o comportamientos inesperados.*

---
---

## Conclusión

Este programa constituye una herramienta robusta para la **optimización** en grafos, integrando:

1. Un **algoritmo de hormigas** que evalúa iterativamente soluciones y actualiza feromonas para guiar la búsqueda.

2. Un **algoritmo evolutivo** que calibra automáticamente los parámetros de control, facilitando y mejorando la búsqueda de soluciones óptimas.

La versatilidad en el ingreso de parámetros–ya sea manual o mediante un sistema automático de calibración–hace del sistema una solución flexible y adaptable a distintos escenarios y tipos de grafos. Su correcto uso dependerá de proveer entradas válidas y respetar el formato señalado para evitar errores en la ejecución

*Stephen Luna Ramírez*

*México, 30 de marzo de 2025*

***Última actualización registrada**: México, 11 de septiembre de 2025*



