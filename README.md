# **Robustez** de Redes Complejas a través de la Inteligencia de Enjambre: Aplicación del Método **Ant System**

El presente documento no busca profundizar en la definición de los conceptos que constituyen el sistema, al estilo de un tratado científico o un ensayo filosófico. En cambio, se ofrece una descripción concisa, similar a un mapa que indica las rutas y métodos para transitar de un origen a un destino (identificados a través de colores, símbolos, etc.). De igual manera, se presenta una guía operativa que detalla el correcto uso del progama: especifica cómo **compilarlo** y **ejecutarlo**, y cómo aplicar las técnicas heurísticas–específicamente, el **Sistema Hormiga** y la **Evolución Diferencial**–sobre una representación de la red.
Esta red modela el problema de optimización (ya sea mediante una fase de **minimización** o **maximización**) de los nodos, dividiéndola en componentes centrales: un conjunto separador, que es el objetivo principal de la solución, y dos componentes mayores, designados como A y B, cuya inclusión resulta indeseable por los elevados costos que implican (o, en el caso de la *maximización*, actúan como restricciones, denominadas también **penalizaciones**).

La **red original** se puede representar mediante dos alternativas, seleccionables por el usuario (ya sea en el programa principal o en el módulo secundario de Evolución Diferencial, un algoritmo heurístico que requiere modificar únicamente un parámetro manualmente): la **matriz de adyacencia** y la **lista de adyacencia**. Ambas opciones modelan el mismo grafo (definido en términos de enlaces y vértices) y tienen como objetivo descomponer la red en los subgrafos descritos previamente para evaluar una función de costo.

En consecuencia, aunque ambas representaciones conducen al mismo resultado, se diferencian en su interpretación y en la eficiencia para identificar los nodos a desconectar, dependiendo de las características de la estructura de datos en términos de **tiempo** y **espacio**. Por ejemplo, en conjuntos extensos de datos, la matriz de adyacencia podría resultar menos eficiente que la lista, que, por otro lado, puede ofrecer una búsqueda y almacenamiento más rápidos. No obstante, el análisis comparativo de la *eficiencia computacional*–medido, por ejemplo, en el **peor caso** (O(grande))–queda fuera del alcance del presente estudio, aunque se persiga optimizar los tiempos de ejecución computacional.

En el presente repositorio se disponen de carpetas que contienen los programas auxiliares necesarios para generar una red específica y realizar su análisis de **robustez**. Dentro de la carpeta denominada *Instancias del Problema* se encuentran los dos conjuntos principales de datos utilizados durante el proyecto (Facebook y Twitter), así como un archivo adicional denominado "grafo_prueba.txt". Este último no forma parte de las instancias originales, sino que se ofrece como modelo de ejemplo para que el usuario visualice rápidamente los resultados sin tener que esperar a obtener una solución completa, lo que optimiza el uso de recursos computacionales, como **memoria** y **procesamiento**.

Al utilizar "grafo_prueba.txt", se solicita al usuario modificar los parámetros de la **Evolución Diferencial**, siguiendo estrictamente el orden establecido de los argumentos, que es el siguiente:

* **Número de iteraciones** (%d, entero positivo)
* **Rho** (%lf, número con/sin decimales; representa el *coeficiente de evaporación*)
* **Alpha** (%lf)
* **Beta** (%lf)
* **Número de hormigas** (%d)
* **Nombre del archivo** (%s; incluir la extensión, por ejemplo, ".txt")
* **Opción de grafo** (%d; 0 para **matriz de adyacencia** y 1 para la **lista de adyacencia**)

**Observación:** Para ejecutar correctamente estos programas auxiliares, se recomienda extraerlos sin conservar la carpeta contenedora original, de manera que todos los archivos queden en un único directorio de ejecución.

Si se desea ejecutar únicamente el **programa principal** (es decir, la aplicación del Sistema Hormiga), el usuario debe ingresar **manualmente** los parámetros requeridos–respetando el orden establecido–en la terminal de su computadora, sobre la cual se procederá a compilar y ejecutar el programa. En caso de que no se proporcione la cantidad exacta de argumentos o se presione simplemente la tecla **ENTER**, la consola indicará las instrucciones pertinentes; sin embargo, el programa finalizará de inmediato al no contar con una entrada válida que permita iniciar su ejecución y generar los resultados esperados.

**Nota:** Se recomienda leer el documento original antes de proceder, pues en él se abordan conceptos y características fundamentales relacionados con las redes y los procesos heurísticos, así como la relevancia de la optimización–analizada en dos contextos: **continua** y **combinatoria**–siendo este último el enfoque principal del estudio.

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

***

## Ejecución del Programa

El programa puede ser ejecutado de dos maneras:

**1. Ejecución Manual**

Se invoca el programa principal **RobustezRed_PT.cpp** a través de su ejecutable (.exe) proporcionando los argumentos (más adelante se explica cómo compilarlo correctamente) obligatorios a través de la línea de comandos. Para este ejemplo, definiremos el ejecutable como **robustez**:

```bash
.\robustez 1 0.5 1.2 1.0 2 grafo_prueba.txt 0
```
* **Numero de iteraciones:** Cantidad de repeticiones del proceso de solución (ejemplo: `1`).

* **Rho:** Factor de evaporación de la feromona (ejemplo: `0.5`).

* **Alpha:** Peso asignado a la influencia de la feromona en el cálculo de probabilidades (ejemplo: `1.2`).

* **Beta:** Peso asignado al valor heurístico (ejemplo: `1.0`).

* **Número de hormigas:** Cantidad de agentes que recorrerán el grafo (ejemplo: `2`).

* **Nombre del archivo:** Archivo que contiene el grafo (por ejemplo, `grafo_prueba.txt`).

* **Opción del grafo:** `0` para que el archivo se interprete como **matriz** y `1` para **lista**.

**Nota:** Si la invocación no incluye todos los parámetros requeridos, o éstos no cumplen con el tipo esperado, el sistema mostrará mensajes indicando el uso correcto de los parámetros, limpiará la pantalla y terminará la ejecución para evitar errores.

**2. Ejecución Automática mediante Evolución Diferencial**

Si no se desea ingresar de manera manual cada parámetro, se puede optar por la ejecución automática utilizando el algoritmo de evolución diferencial. El proceso es el siguiente:

1. **Compilación del algoritmo de evolución (DE):**

```bash
gcc .\DE_AntSystem.c -o DE
```
2. **Compilación del sistema de hormigas y sus herramientas (funciones necesarias):**

```bash
g++ .\Herramientas.cpp RobustezRed_PT.cpp -o robustez
```

3. **Ejecución del algoritmo evolutivo:**

```bash
.\DE
```

Dentro del código del algoritmo evolutivo (**DE_AntSystem.c**) se utiliza una instrucción `snprintf()` en la función denominada `evaluar()` para generar el comando de ejecución del sistema de hormigas:

> `snprintf(comando, sizeof(comando), ".\\robustez.exe %d %lf %lf %lf %d %s %d > resultado.txt", ind.iteraciones, ind.rho, ind.alpha, ind.beta, ind.num_hormigas, "grafo_prueba.txt", 0);`

**IMPORTANTE:**

* Si se utiliza **Linux** o **MacOS**, se debe cambiar la cadena `".\\robustez.exe"` por `"./robustez.exe"`.

* Para **Windows**, se mantiene el formato `".\\robustez.exe"`

Aquí, sólo se requiere que el usuario especifique el **nombre del archivo** y la **opción para el grafo**, mientras que el resto de los argumentos son generados automáticamente por el **algoritmo evolutivo**.

***
## Entradas Válidas y No Válidas

* **Entradas Válidas:**

    - **Números:** Los argumentos numéricos (**iteraciones**, **Rho**, **Alpha**, **Beta** y **número de hormigas**) deben ser del tipo adecuado (por ejemplo, **enteros** o **flotantes** según se requiera).

    - **Archivo:** El nombre del archivo debe corresponder a un fichero existente con el formato correcto (por ejemplo, "grafo_prueba.txt").

    - **Opción del grafo:** Sólo se aceptan `0` (para el grafo en forma de matriz) o `1` (para el grafo en forma de lista).

* **Entradas No Válidas:**

    - **Parámetros Incorrectos o Faltantes:** La omisión de algún argumento o proporcionar datos de un tipo inadecuado (por ejemplo, texto en lugar de un número).

    - **Archivo Inválido:** Un archivo inexistente o con un formato que no se pueda intepretar correctamente.

    - **Opción Incorrecta:** Cualquier valor distinto a `0` o `1` para la opción del grafo (red, formalmente).

*El sistema **valida** los argumentos al **inicio de la ejecución**; en caso de detectar inconsistencias, notificará al usuario con un mensaje descriptivo y finalizará la ejecución de manera segura, evitando posibles errores o comportamientos inesperados.*

***

## Conclusión

Este programa constituye una herramienta robusta para la **optimización** en grafos, integrando:

1. Un **algoritmo de hormigas** que evalúa iterativamente soluciones y actualiza feromonas para guiar la búsqueda.

2. Un **algoritmo evolutivo** que calibra automáticamente los parámetros de control, facilitando y mejorando la búsqueda de soluciones óptimas.

La versatilidad en el ingreso de parámetros–ya sea manual o mediante un sistema automático de calibración–hace del sistema una solución flexible y adaptable a distintos escenarios y tipos de grafos. Su correcto uso dependerá de proveer entradas válidas y respetar el formato señalado para evitar errores en la ejecución

*Stephen Luna Ramírez*

*México, 30 de marzo de 2025*

***

