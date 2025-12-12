<div align="center">
<h1>Implementación del Algoritmo Needleman-Wunsch 📈</h1>
</div>

**Nombres:** Ciel Pérez, Elia Ulloa <br>
**Asignatura:** Algoritmos y Estructura de Datos <br>
**Carrera:** Ingeniería Civil en Bioinformática <br>

<div align="center">
<strong>🎓Universidad de Talca - Facultad de Ingeniería🎓</strong>
</div>

---

<br>

<div align="center">
<h2>Descripción del Proyecto</h2>
</div>

Este proyecto implementa el **Algoritmo de Needleman-Wunsch** en C++ para realizar el **alineamiento global** óptimo entre dos secuencias biológicas (ADN).

El algoritmo utiliza la técnica de **Programación Dinámica** para asegurar una solución óptima y encontrar el puntaje máximo que se logra al alinear las dos cadenas de ADN.

El alfabeto utilizado es $A = \{^{\prime}A^{\prime}, ^{\prime}G^{\prime}, ^{\prime}C^{\prime}, ^{\prime}T^{\prime}\}$.

---

### 🔹 Requisitos

- Se requiere un **Compilador C++** compatible (como `g++`).
- **Graphviz** para generar el `.png` del alineamiento junto a su respectivo archivo `.dot`.
- [Opcional] ImageMagick + Pango para guardar la matriz de programación dinámica de NW (con un máximo de 54 nucleótidos).
- `limpiarArchivos.bash` debe estar en la misma carpeta que el ejecutable `needlemanWunsch` para funcionar.
- Se necesitan 2 secuencias (C1 y C2) y la matriz `matrizPuntuacion.csv`.

### 🔹 Compilación
El programa requiere **cuatro parámetros de entrada**. Para compilar el programa se utiliza:
```
./needlemanWunsch -C1 SECUENCIA1.fna -C2 SECUENCIA2.fna -U matrizPuntuacion.csv -V VALORNEGATIVO
```

`-C1 SECUENCIA1.fna` = Secuencia horizontal <br>
`-C2 SECUENCIA2.fna` = Secuencia vertical <br>
`-U matrizPuntuacion.csv` = Matriz de puntuación <br>
`-V VALORNEGATIVO`  = Valor de penalización por gap <br>

Un ejemplo directo para correr el programa con lo que ya está en el repositorio es:
```
./needlemanWunsch -C1 HUMAN.fna -C2 RAT.fna -U matrizPuntuacion.csv -V -2
```

---

<br>

<div align="left">
<h3>Estructura de Archivos</h3>
</div>

📂 [Proyecto_Algoritmos_2025] <br>
 ┣ `README.md` <br>
 ┣ `needlemanWunsch.cpp` ----> Código fuente principal <br>
 ┣ `needlemanWunsch` --------> Ejecutable <br>
 ┣ `limpiarArchivos.bash` ---> Script para limpiar archivos FASTA (mantiene 'ATCG'). Resultados en carpeta FASTAS <br>
 ┣ `matrizPuntuacion.csv` ---> Puntuación de missmatch y gap <br>
 ┗ 📂 `FASTAS/`  Archivos de secuencias de entrada (.fna) creadas por el bash <br>

<br>

---

<div align="center">
<h2>Lógica del Algoritmo Needleman-Wunsch</h2>
</div>



CONTINUELATER
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
