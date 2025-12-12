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
  ```
  sudo apt-get install graphviz imagemagick pango
  ```
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
<h3>🔷 Estructura de Archivos</h3>
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

<div align="left">
<h3>🔷 Flujo del programa</h3>
</div>

1. **Limpieza y validación de FASTA**  
   El script `limpiarArchivos.bash` garantiza que las secuencias contengan solo nucleótidos válidos (A, T, C, G).  
   Si una secuencia no cumple el formato, el bash genera una versión corregida terminada en `_CLEAN-SHORT.fna`.

2. **Carga de la matriz de puntuación desde el CSV**  
   Se leen los encabezados (A, T, C, G) y luego se construye una matriz U(c,d) donde cada par de nucleótidos entrega un puntaje.  
   Esta matriz define **matches/mismatches** durante el alineamiento.

3. **Inicialización de la matriz de Programación Dinámica**  
   Se crea una matriz `f(i,j)` de tamaño `(len(C2)+1) x (len(C1)+1)`  
   Las primeras fila/columna representan alinear una secuencia con *solo gaps*, usando el valor de penalización `V`.  
   - `f(i,0) = i * V`  
   - `f(0,j) = j * V`

4. **Llenado de la matriz NW (Programación Dinámica)**  
   Cada celda f(i,j) se calcula como el máximo entre tres opciones:
   f(i-1,j-1) + U(C2[i-1], C1[j-1]) ← Diagonal (Match / Mismatch)
   f(i-1,j) + V ← Gap en C1
   f(i,j-1) + V ← Gap en C2
   
   Además se guarda en `matrizDir[i][j]` la dirección que produjo ese máximo:
   - `0` = diagonal  
   - `1` = arriba  
   - `2` = izquierda

5. **Backtracking desde f(n,m)**  
   Con `matrizDir` se reconstruye el alineamiento óptimo, moviéndose:
   
   - Diagonal → empareja ambos caracteres  
   - Arriba → gap en Cadena 1  
   - Izquierda → gap en Cadena 2  
   
   Este proceso recorre el **camino óptimo** y genera C1_alineado y C2_alineado.

6. **Estadísticas del alineamiento**  
   Se calculan:
   - Matches  
   - Mismatches  
   - Gaps  
   - Porcentaje de similitud  
   
   Todo basado en las dos secuencias alineadas ya reconstruidas.

7. **Generación de visualizaciones**  
   El programa ofrece tres salidas opcionales:
   - `matrizNW.png` → Renderizado de la matriz de programación dinámica (requiere ImageMagick + Pango).  
   - `backtrack.svg` → Visualización completa de la matriz NW con flechas de backtracking.  
   - `alineamiento_resultado.png` → Gráfico del alineamiento (Graphviz), coloreando  
     - verde (match)  
     - rojo (mismatch)  
     - gris (gap)

<br>

---

<div align="left">
<h3>💬 Otras cosas a mencionar</h3>
</div>

- `userDecision()` viene de combinar los códigos presentes en [esta página](https://stackoverflow.com/questions/43972500/how-to-only-accept-y-or-n-in-users-input-in-c)
- Para generar la imágen `matrizNW.png` las secuencias debe ser máximo de 54 nucleótidos, igualmente, no dejará que se genere si esto no se cumple.
- No se realizarán los pasos del bash de limpieza si el archivo seleccionado tiene en el nombre escrito '_CLEAN-SHORT.fna'.
- Se utiliza SVG para generar la imágen de backtrack ya que graphviz se tarda demasiado en realizar este trabajo.

