# Implementación del Algoritmo Needleman-Wunsch

## Autor

**Nombres:** Ciel Pérez, Elia Ulloa
**Asignatura:** Algoritmos y Estructura de Datos
**Carrera:** Ingeniería Civil en Bioinformática
**Universidad de Talca - Facultad de Ingeniería**

---

## Descripción del Proyecto

Este proyecto implementa el **Algoritmo de Needleman-Wunsch** en C++ para realizar el **alineamiento global** óptimo entre dos secuencias biológicas (ADN).

El algoritmo utiliza la técnica de **Programación Dinámica** para asegurar una solución óptima y encontrar el puntaje máximo que se logra al alinear las dos cadenas de ADN.

El alfabeto utilizado es $A = \{^{\prime}A^{\prime}, ^{\prime}G^{\prime}, ^{\prime}C^{\prime}, ^{\prime}T^{\prime}\}$.

---

### Requisitos

* **Compilador C++:** Se requiere un compilador compatible con C++ (como `g++`).
* **Graphviz:** El software `dot` debe estar instalado y en el PATH del sistema para generar automáticamente el diagrama de alineamiento PNG.

### Estructura de Archivos

📂 [Proyecto_Algoritmos_2025]
 ┣ README.md
 ┣ needlmanWunsch.cpp  # Código fuente principal
 ┣ limpiarArchivos.bash # Script para preprocesar archivos FASTA
 ┣ 📂 FASTAS/           # Archivos de secuencias de entrada (.fna)
 ┗ 📂 bin/              # (Directorio de salida para ejecutables)

### Ejecución del Programa

El programa requiere **cuatro parámetros de entrada**: las dos secuencias, la matriz de puntuación U, y el valor de penalización V.

#### Sintaxis de Ejecución bash
```bash
./programa -C1 [archivo_C1].fna -C2 [archivo_C2].fna -U [matriz_U].csv -V [valor_V]

Ejemplo de comado:

g++ needlemanWunsch.cpp -o needlemanWunsch 
./needlemanWunsch -C1 HUMAN.fna -C2 RAT.fna -U matrizPuntuacion.csv -V -2

---
#### Lógica del Algoritmo Needleman-Wunsch

El algoritmo se basa en el llenado de una matriz de Programación Dinámica f(i,j).
1. Cálculo del Puntaje (Score)

El valor de cada celda f(i,j) (para i>0 y j>0) es el máximo de tres opciones posibles:

    Alineamiento por Diagonal (Match / Mismatch): Emparejar S[i] con T[j].
    f(i−1,j−1)+U(S[i],T[j])

    Alineamiento Superior (Gap en S): Insertar un gap (-) en la Cadena 1 (S).
    f(i−1,j)+V

    Alineamiento Izquierdo (Gap en T): Insertar un gap (-) en la Cadena 2 (T).
    f(i,j−1)+V

El puntaje máximo del alineamiento global se encuentra en la celda f(n,m).
2. Lógica de Penalizaciones y Puntuación

    Matriz de Puntuación U(c,d): Da como resultado el puntaje de similitud entre dos letras del alfabeto, definiendo el Match o Mismatch.

    Valor de Penalización V: Es el puntaje fijo que se asigna cuando se introduce un gap (no emparejamiento) en cualquiera de las secuencias. Este valor es negativo.

3. Fase de Reconstrucción (Backtrack)

El Backtrack reconstruye el alineamiento que da el puntaje más alto, recorriendo el camino óptimo desde f(n,m) hasta f(0,0).

    Diagonal: Se recorre si f[i][j] proviene de f[i−1][j−1]+U[S[i]][T[j]], resultando en un emparejamiento de caracteres (S[i] con T[j]).

    Arriba: Se recorre si f[i][j] proviene de f[i−1][j]+V, resultando en un gap en la Cadena 1.

    Izquierda: Se recorre si f[i][j] proviene de f[i][j−1]+V, resultando en un gap en la Cadena 2.

El resultado final son las dos cadenas alineadas con gaps insertados.

 Visualización del Resultado

El programa cumple con el requerimiento de entregar la reconstrucción del alineamiento óptimo utilizando Graphviz.
Proceso de Visualización

    El código genera el archivo alineamiento_resultado.dot.

    Representa el alineamiento final como una secuencia lineal de nodos, donde cada nodo es un nucleótido o un gap.

    Utiliza colores para indicar el estado de la alineación en cada posición:

        Verde: Match (Coincidencia).

        Rojo: Mismatch (Desigualdad).

        Gris: Gap (Brecha).

    El programa usa system para generar la imagen alineamiento_resultado.png.
