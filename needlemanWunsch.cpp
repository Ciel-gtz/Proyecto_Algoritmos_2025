#include <vector> // Para: vector + push_back()
#include <sstream> // Para: stringstream
#include <fstream> // Para: ifstream, ofstream
#include <iostream> // Para: cin, cout, cerr
#include <string> // Para: to_string(), string, getline()
#include <algorithm>  // Para: max, reverse, transform
#include <limits> // Para: numeric_limits
#include <cstdlib>    // Para: system -> generar png

using namespace std;

// ─────────────| Pedir decisión al usuario |─────────────¬

char userDecision() {
    char userAnswer;
    do {
        cout << "! [s/n] : ";
        cin >> userAnswer;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        userAnswer = (char)tolower(userAnswer);
    } 
    while( !cin.fail() && userAnswer!='s' && userAnswer!='n' );

    return userAnswer;   
}


// ─────────────| Sobre los archivos |─────────────¬

// | Corre el bash
int limpiarArchivos(string nombre_archivo){
	string run = "bash limpiarArchivos.bash " + nombre_archivo;
	int status = system(run.c_str());

	/*  ─────────────────────────────
	return 0 = Corrió el script correctamente
	return 1 = Error al leer el archivo
	return 2 = Archivo ya tiene el formato _CLEAN-SHORT
	    ─────────────────────────────  */

	if (status == 256) { // 256 es equivalente a exit 1 de bash [256 x exitvalue]
		cout << "!\tError con el archivo " << nombre_archivo << endl;
		exit(1);
	} 
    
    else if (status == 512) { // 512 es equivalente a exit 2 de bash
		cout << "!\tEl archivo " << nombre_archivo << " ya tiene el formato _CLEAN-SHORT.fna" 
			 << "\n\t Se utilizará la longitud predeterminada del archivo." 
             << "\n!\t  Si desea cambiar la longitud: borre del nombre del archivo '_CLEAN-SHORT' y corra este código otra vez.\n" << endl;
		return 2;
	}

	return 0;
}


// | Actualiza el nombre del archivo en el caso 0 de limpiarArchivos()
string actualizarNombreArchivo(string nombre_archivo) {
	// Remueve '.fna' del nombre de la variable
	string nuevo_nombre = nombre_archivo.substr(0, nombre_archivo.size() - 4);

	// Agrega '_CLEAN-SHORT.fna' a la variable + la carpeta donde los archivos limpios se guardan
	return "FASTAS/" + nuevo_nombre + "_CLEAN-SHORT.fna";
}


// | Guarda la información del archivo en una string
string guardarInfo(string nombre_archivo) {
	string secuencia;
	string linea;


	ifstream archivo(nombre_archivo);

	// Error de lectura
	if (!archivo) {
        cerr << "\n!\tNo se pudo abrir el archivo " << nombre_archivo << endl;
        exit(1);
    }

	// Obtiene todas las líneas del archivo y las guarda en una sola string
	while (getline(archivo, linea)) {
		secuencia += linea;
	}

	archivo.close();
	return secuencia;
}


// ─────────────| Lectura de CSV (con salto de encabezados) |─────────────¬

vector<vector<int>> leerCSV(const string& nombre_archivo, vector<string>& etiquetasFila) {
    vector<vector<int>> matriz;
    ifstream archivo(nombre_archivo);

    if (!archivo.is_open()) {
        cerr << "!\tNo se pudo abrir el archivo CSV: " << nombre_archivo << endl;
        return matriz;
    }

    string linea;

    // Leer la primera fila (encabezados de columnas) y descartarla aquí.
    if (!getline(archivo, linea)) {
        cerr << "!\tCSV vacio: " << nombre_archivo << endl;
        return matriz;
    }

    // Ahora leer cada fila: la primera celda de cada fila es la etiqueta de la fila (A/T/C/G)
    while (getline(archivo, linea)) {
        stringstream lineaCSV(linea);
        string casilla;

        vector<int> fila;
        int columnaIdx = 0;

        // Leer la primera celda (etiqueta de fila)
        if (!getline(lineaCSV, casilla, ',')) continue;

        // Guardar la etiqueta de fila (sin espacios)
        string etiqueta = casilla;
        etiquetasFila.push_back(etiqueta);

        // Leer el resto de casillas separadas por coma (valores numéricos)
        while (getline(lineaCSV, casilla, ',')) {
            try {
                fila.push_back(stoi(casilla));
                // Esto porque se leen secuencialmente ➜ se quieren agregar secuencialmente ("de arriba para abajo")
            } catch (...) {
                // Valor inválido ➜ ignorar
            }
            columnaIdx++;
        }

        matriz.push_back(fila);
    }

    return matriz;
}


// ─────────────| Función para leer encabezados desde el CSV |─────────────¬

// | Encabezado de fila y de columna para hacer la comparacion entre (A, T, C, G)
vector<string> leerEncabezados(const string &nombreCSV) {
    ifstream file(nombreCSV);
    vector<string> encabezados;
    string linea;

    // Error al abrir el archivo CSV
    if (!file.is_open()) {
        cerr << "!\tNo se pudo abrir el archivo: " << nombreCSV << endl;
        return encabezados;
    }

    // Leer únicamente la primera línea del CSV (la fila de encabezados)
    if (!getline(file, linea)) {
        // Error al leer la línea
        return encabezados;
    } 

    // Procesar la línea como si fuera un archivo con stringstream para usar getline()
    stringstream ss(linea);
    string casilla;

    // Leer la primera celda (vacía) y descartarla
    getline(ss, casilla, ',');

    // Leer el resto de celdas A, T, C, G (en cualquier orden en el que estén)
    while (getline(ss, casilla, ',')) {
        // Evitar agregar celdas vacías (ej. si hay comas dobles ",,")
        if (!casilla.empty()){
            encabezados.push_back(casilla);
        }
    }

    return encabezados;
}


// ─────────────| Sobre Needleman-Wunsch |─────────────¬

// | Función para inicializar la matriz Needleman-Wunsch
vector<vector<int>> inicializarMatrizNW(int filas, int columnas, int penal) {
    // La matriz de puntuación (f(i, j)) tendrá tamaño (n+1) x (m+1)
    vector<vector<int>> matriz(filas, vector<int>(columnas, 0));

    // Inicialización del caso base: f(i, 0) = V * i 
    for (int i = 1; i < filas; i++)
        matriz[i][0] = matriz[i-1][0] + penal; // sumar penal que ya es negativo

    // Inicialización del caso base: f(0, j) = V * j
    for (int j = 1; j < columnas; j++)
        matriz[0][j] = matriz[0][j-1] + penal; // sumar penal que ya es negativo

    return matriz;
}


// | Función de Puntuación (Score) 
int scoreNW(char filaCaracter, char colCaracter, const vector<string> &etiquetasFila, const vector<string> &etiquetasColumna, const vector<vector<int>> &mat) {
    int i = -1, j = -1;

    // Buscar el índice del nucleótido en las listas de encabezados
    for (int k = 0; k < etiquetasFila.size(); k++) {
        if (etiquetasFila[k].size() == 1 && etiquetasFila[k][0] == filaCaracter) { i = k; break; }
    }
    for (int k = 0; k < etiquetasColumna.size(); k++) {
        if (etiquetasColumna[k].size() == 1 && etiquetasColumna[k][0] == colCaracter) { j = k; break; }
    }

    // Si i o j no se encontraron
    if (i == -1 || j == -1) {
        cerr << "!\tNucleotido no encontrado (" << filaCaracter << ", " << colCaracter << ")\n";
        exit(1); // Salir del programa
    }
    
    return mat[i][j];
}


// | Construye la matriz de puntuación de Needleman–Wunsch y la matriz de direcciones.
void llenarMatrizNW(const string &C1, const string &C2, vector<vector<int>> &matrizNW, vector<vector<int>> &matrizDir, int V, 
                    const vector<string> &etiquetasFila, const vector<string> &etiquetasColumna, const vector<vector<int>> &matPunt) {

    int filas = matrizNW.size();        // filas = largo de C2 + 1
    int columnas = matrizNW[0].size();  // columnas = largo de C1 + 1

    // Se excluye fila y columna inicial
    for (int i = 1; i < filas; i++) {
        for (int j = 1; j < columnas; j++) {
        
           /*  ─────────────────────────────
            ➜ i avanza sobre C2 (vertical)
            ➜ j avanza sobre C1 (horizontal)
            el caracter de la "fila" es C2[i-1]   ;   el de la "columna" es C1[j-1]
                ─────────────────────────────  */

            int diagonal = matrizNW[i-1][j-1] + scoreNW(C2[i-1], C1[j-1], etiquetasFila, etiquetasColumna, matPunt);
            int arriba   = matrizNW[i-1][j] + V; // Inserta gap en C1
            int izquierda = matrizNW[i][j-1] + V; // Inserta gap en C2

            // Determinar el valor máximo.
            int mejorValor = diagonal;
            if (arriba > mejorValor) mejorValor = arriba;
            if (izquierda > mejorValor) mejorValor = izquierda;

            matrizNW[i][j] = mejorValor;

            // Guardar de dónde vino el valor.
            if (mejorValor == arriba)
                matrizDir[i][j] = 1;      // arriba = 1
            else if (mejorValor == izquierda)
                matrizDir[i][j] = 2;      // izquierda = 2
            else
                matrizDir[i][j] = 0;      // diagonal = 0
        }
    }
}


// | visualización de la matriz NW
void imprimirMatriz(const vector<vector<int>>& matriz, const string& C1, const string& C2) {    
    // Imprimir encabezado de columnas (Secuencia C1)
    cout << "\t\t ─";
    for (char c1 : C1) {
        cout << "\t│" << c1;
    }
    cout << "\n";

    // Imprimir filas
    for (int i = 0; i < matriz.size(); ++i) {

        // Imprimir encabezado de fila (Secuencia C2)
        if (i == 0) {
            cout << "\t─ ";
        } else {
            cout << "\t" << C2[i-1] << "   ➜";
        }
        
        // Imprimir valores de la matriz
        for (int j = 0; j < matriz[i].size(); ++j) {
            cout << "\t│" << matriz[i][j]; 
        }
        cout << "\n";
    }
    cout << endl;
}


// ─────────────| Backtrack |─────────────¬

// pair<C1,C2> contiene 2 valores:  C1 = alineación resultante de C1;   C2 = alineación resultante de C2
pair<string, string> backtrackNW(const vector<vector<int>>& matrizDir, const string& C1, const string& C2) {
    // Comienza desde la esquina inferior derecha
    int i = C2.size();  // filas
    int j = C1.size();  // columnas

    string seq1;   // alineación resultante de C1
    string seq2;   // alineación resultante de C2

    // Retrocedemos mientras queden caracteres de alguna secuencia
    while (i > 0 || j > 0) {

        // Caso borde: estamos en la primera columna ➜ sólo podemos subir
        if (j == 0) {
            seq1 += '-';
            seq2 += C2[i - 1];
            i--;
            continue;
        }

        // Caso borde: estamos en la primera fila ➜ sólo podemos ir a la izquierda
        if (i == 0) {
            seq1 += C1[j - 1];
            seq2 += '-';
            j--;
            continue;
        }

        // Dirección codificada en matrizDir[i][j]
        int dir = matrizDir[i][j];

        // | Diagonal ➜ Se emparejan ambos caracteres
        if (dir == 0) {
            seq1 += C1[j - 1];
            seq2 += C2[i - 1];
            i--; j--;
        }
        // | Arriba ➜ Gap en C1
        else if (dir == 1) {
            seq1 += '-';
            seq2 += C2[i - 1];
            i--;
        }
        // | Izquierda ➜ Gap en C2
        else if (dir == 2) {
            seq1 += C1[j - 1];
            seq2 += '-';
            j--;
        }
        else {
            cerr << "!\tDirección invalida.\n";
            exit(1);
        }
    }

    // Se construyó al revés ➜ ahora se invierte
    reverse(seq1.begin(), seq1.end());
    reverse(seq2.begin(), seq2.end());

    return {seq1, seq2};
}


// ─────────────| Para mostrar información |─────────────¬

int contarMatches(const string &C1, const string &C2) {
    int matches = 0;

    for (int i = 0; i < C1.size(); i++) {
        if (C1[i] == '-' || C2[i] == '-') continue;
        if (C1[i] == C2[i]) matches++;
    }

    return matches;
}

void contarMismatchesGapsPorcentaje(const string &C1, const string &C2, int &mismatches, int &gaps, double &porcentaje) {
    mismatches = 0;
    gaps = 0;
    int comparables = 0;

    for (int i = 0; i < C1.size(); i++) {
        char c1 = C1[i];
        char c2 = C2[i];

        // Cuenta gaps
        if (c1 == '-' || c2 == '-') {
            gaps++;
            continue;
        }

        comparables++;

        // Cuenta missmatches
        if (c1 != c2)
            mismatches++;
    }

    // Calcula el porcentaje de similitud
    if (comparables > 0)
        porcentaje = (double)(comparables - mismatches) / comparables * 100.0;
    else
        porcentaje = 0.0;
}


// ─────────────| Para generar imágen de la matriz [Se necesita tener instalado ImageMagick con Pango] |─────────────¬

// | Guarda la matriz en un archivo txt para luego convertirla en imagen
void guardarMatrizEnTxt(const vector<vector<int>>& matriz, const string& C1, const string& C2, const string& nombreArchivo) {
    ofstream out(nombreArchivo);
    
    if (!out.is_open()) {
        cerr << "No se pudo crear archivo: " << nombreArchivo << "\n";
        return;
    }

    // Encabezado
    out << "\t ─";
    for (char c1 : C1) out << "\t│" << c1;
    out << "\n";

    // Filas
    for (int i = 0; i < matriz.size(); ++i) {

        if (i == 0) out << " ";
        else        out << "" << C2[i-1] << "   ➜";

        for (int j = 0; j < matriz[i].size(); ++j) {
            out << "\t│" << matriz[i][j];
        }

        out << "\n";
    }
}

// | Convierte el txt generado en una imagen PNG [MAX = 54]
void convertirTxtAPNG(const string& txt, const string& png) {
    string bgHex = "#2b213fff"; // Fondo blanco 
    string textHex = "#e8dceeff"; //Letra negra
    int padding = 200; // Padding en píxeles

    string cmd =
        "bash -c \""
        "convert -background '" + bgHex + "' "
        "-fill '" + textHex + "' "
        "-font DejaVu-Sans-Mono -pointsize 14 "
        "-density 300 "
        "-bordercolor '" + bgHex + "' "
        "-border " + to_string(padding) + " " +
        "pango:\\\"<span font='DejaVu Sans Mono 14'>$(cat " + txt + ")</span>\\\" "
        + png +
        "\"";

        // Silenciar TODA la salida del comando
        cmd += " > /dev/null 2>&1";
        
    int status = system(cmd.c_str());
    if (status != 0) {
        cerr << "!\tError al generar imagen de la matriz ; " << status 
             << "\n\t Revisa que ImageMagick + Pango esten instalados.\n";
    } else {
        cout << "\n> Imagen generada: matrizNW.png\n";
    }
}







// | Genera el código DOT y ejecuta 'dot' para generar el PNG
void generarGraphviz(const string& seq1, const string& seq2, int score, const string& nombreArchivo) {
    
    ofstream dotFile(nombreArchivo);
    if (!dotFile.is_open()) {
        cerr << "!\tError: No se pudo crear el archivo DOT: " << nombreArchivo << endl;
        return;
    }
    
    dotFile << "digraph AlignmentResult {\n";
    dotFile << "  rankdir=LR; // Izquierda a Derecha\n"; 
    dotFile << "  label = \"Alineamiento Óptimo Needleman-Wunsch\\nPuntaje: " << score << "\";\n";
    dotFile << "  labelloc = \"t\";\n\n";

    // Definir subgrafo para la secuencia 1 (C1)
    dotFile << "  subgraph cluster_C1 {\n";
    dotFile << "    label = \"Cadena 1\";\n";
    dotFile << "    rank = same;\n";
    dotFile << "    node [shape=box, style=filled, height=0.3, width=0.3, fixedsize=true];\n";

    // Nodos para C1 y enlaces secuenciales
    for (size_t k = 0; k < seq1.length(); ++k) {
        string nodeName = "C1_" + to_string(k);
        string color;
        
        if (seq1[k] == '-' || seq2[k] == '-') {
            color = "gray"; // Gap
        } else if (seq1[k] == seq2[k]) {
            color = "green"; // Match
        } else {
            color = "red"; // Mismatch
        }
        
        dotFile << "    " << nodeName << " [label=\"" << seq1[k] << "\", fillcolor=" << color << ", group=g" << k << "];\n";
        if (k > 0) {
            dotFile << "    C1_" << to_string(k-1) << " -> " << nodeName << " [dir=none];\n";
        }
    }
    dotFile << "  }\n\n";

    // Definir subgrafo para la secuencia 2 (C2)
    dotFile << "  subgraph cluster_C2 {\n";
    dotFile << "    label = \"Cadena 2\";\n";
    dotFile << "    rank = same;\n";
    dotFile << "    node [shape=box, style=filled, height=0.3, width=0.3, fixedsize=true];\n";
    
    // Nodos para C2 y enlaces secuenciales
    for (size_t k = 0; k < seq2.length(); ++k) {
        string nodeName = "C2_" + to_string(k);
        string color;

        if (seq1[k] == '-' || seq2[k] == '-') {
            color = "gray"; // Gap
        } else if (seq1[k] == seq2[k]) {
            color = "green"; // Match
        } else {
            color = "red"; // Mismatch
        }

        dotFile << "    " << nodeName << " [label=\"" << seq2[k] << "\", fillcolor=" << color << ", group=g" << k << "];\n";
        if (k > 0) {
            dotFile << "    C2_" << to_string(k-1) << " -> " << nodeName << " [dir=none];\n";
        }
    }
    dotFile << "  }\n\n";
    
    dotFile << "}";
    dotFile.close();
    
    string outputPNG = "alineamiento_resultado.png";
    string command = "dot -Tpng " + nombreArchivo + " -o " + outputPNG;
    
    // Ejecutar el comando para crear el PNG
    int status = system(command.c_str());
    
    // Notificar al usuario
    cout << "\n" << "────────────────────────────────────────────" << "\n";
    cout << "  VISUALIZACIÓN GENERADA AUTOMÁTICAMENTE" << "\n";
    cout << "────────────────────────────────────────────" << "\n";
    cout << "  Archivo DOT generado: '" << nombreArchivo << "'.\n";
    
    if (status == 0) {
        cout << "  Archivo PNG generado: '" << outputPNG << "'.\n";
    } else {
        cerr << "  Error al generar el PNG. Asegúrese de que Graphviz esté instalado.\n";
    }
    cout << "  Colores: Match (verde), Mismatch (rojo) o Gap (gris).\n";
    cout << "────────────────────────────────────────────" << "\n";
}


// ─────────────| Main |─────────────¬

int main(int argc, char** argv) {
	
	// <─────────────| Valores iniciales |─────────────>

	string C1, C2, U; 
	int V, limite, valor1, valor2; 
    string nucleotidos[4] = {"A", "T", "C", "G"};

	/*  ─────────────────────────────
	C1 = Secuencia horizontal | fna
	C2 = Secuencia vertical | fna
	U  = Matriz de puntuación | csv
	V  = Valor de penalización por gap
	limite = Longitud máxima de las cadenas
	valor1, valor2 = Valor temporal para validaciones
	    ─────────────────────────────  */


	// <─────────────| Validación de argumentos de entrada |─────────────>

	// | El usuario ingresó menos o más argumentos de los necesarios ➜ sale del programa
	if (argc < 9 || argc > 9) { 
		cout << "!\tFaltan o sobran argumentos (DEBEN SER 9 ARGUMENTOS)" 
			 << "\n\tUso: ./needlemanWunsch -C1 [secuenciaH].fna -C2 [secuenciaV].fna -U matrizPuntuacion.csv -V [valor]" 
			 << endl;
        return 1;
    }

	// | Recorre los argumentos ingresados por el usuario
	for (int i = 1; i < argc; i++) {

		string actual = argv[i];

		// Si el argumento actual [i] es == -C1 ➜ Se guarda el siguiente argumento como C1
        if (actual == "-C1")  {
            C1 = argv[++i];
        } 
        else if (actual == "-C2") {
            C2 = argv[++i];
        } 
        else if (actual == "-U") {
            U = argv[++i];
        } 
        else if (actual == "-V") {
            V = atoi(argv[++i]);

			// Verifica que V sea un número menor a 0
			if ((V == 0) || (V >= 0)) {
                cout << "!\tEl valor de penalización por gap (-V) debe ser menor a 0.\n";
                return 1;
            }
        }
    }


	// | Algún argumento no tiene valor asignado ➜ sale del programa
	if (C1.empty() || C2.empty() || U.empty()) {
		cout << "!\tAlgun argumento falta (-C1 = " << C1 << ", -C2 = " << C2 << ", -U = " << U << ", -V = " << V << ")" 
			 << "\n\tUso: ./needlemanWunsch -C1 [secuenciaH].fna -C2 [secuenciaV].fna -U matrizPuntuacion.csv -V [valor]" 
			 << endl;
        return 1;
    }


	// <─────────────| Sobre limpiar archivos |─────────────>
	
	// | Se limpian archivos con script bash en caso de que no tengan el formato correspondiente (ATCG)
	valor1 = limpiarArchivos(C1);
	valor2 = limpiarArchivos(C2);
	
	// | Se renombran los archivos si es que fueron limpiados
	if (valor1 == 0) {
		C1 = actualizarNombreArchivo(C1);
	} 
	if (valor2 == 0) {
		C2 = actualizarNombreArchivo(C2);
	}


	// <─────────────| Sobre lectura de archivos |─────────────>

	// | C1 y C2 Pasan de ser nombres de archivos a ser las secuencias leídas de estos archivos
	C1 = guardarInfo(C1);
	C2 = guardarInfo(C2);
	
    cout << "  ────────────────────¬\n│ Secuencias a alinear:"
		 << "\n│ Cadena 1: " << C1 << "\n│ Cadena 2: " << C2 << "\n" << endl;


   // <─────────────| Leer y Verificar CSV |─────────────>

    vector<string> etiquetasFila;
    vector<vector<int>> matrizPuntuacion = leerCSV(U, etiquetasFila);
    vector<string> encabezados = leerEncabezados(U); 

    if (matrizPuntuacion.empty() || encabezados.empty() || etiquetasFila.empty()) {
        cerr << "!\tError al cargar la matriz de puntuación o encabezados.\n";
        return 1;
    }

    // | Impresión de matrizPuntuacion.csv (usando etiquetasFila para filas)
    cout << "\n\t<──| Matriz de puntuacion (U) |──>\n\n\t  ─\t" ;

    for (const string& header : encabezados) {
        cout << "│" << header << "\t";
    } cout << "\n";

    for (int r = 0; r < (int)matrizPuntuacion.size(); ++r) {  
        cout << "\t│ " << ( (r < (int)etiquetasFila.size() && !etiquetasFila[r].empty()) ? etiquetasFila[r] : nucleotidos[r] ); 

        for (int c = 0; c < (int)matrizPuntuacion[r].size(); ++c) {
            cout << "\t│" << matrizPuntuacion[r][c]; 
        }

        cout << "\n";
    } cout << endl;


    // <─────────| Ejecución del Algoritmo NW |─────────>

	int filas = C2.size() + 1;
    int columnas = C1.size() + 1;

    // 1. Inicialización de la matriz (Ceros en M[0][0] y llenado de bordes con penalización).
    vector<vector<int>> matrizNW = inicializarMatrizNW(filas, columnas, V);
    vector<vector<int>> matrizDir(filas, vector<int>(columnas, -1)); // Matriz para direcciones (backtrack)

    // 2. Llenado de la matriz (Programación Dinámica).
    llenarMatrizNW(C1, C2, matrizNW, matrizDir, V, etiquetasFila, encabezados, matrizPuntuacion);
    
    // 3. Impresión de la matriz resultante.
    cout << "\n> ¿Desea mostrar la matriz de programación dinámica (NW)?: ";
    if (userDecision() == 's') {
        cout << "\n\t\t   -─────────────| Matriz de Programación Dinámica [NW] |─────────────-\n" << endl;
        imprimirMatriz(matrizNW, C1, C2);
    } 

    // Guardar siempre la matriz en formato TXT
    guardarMatrizEnTxt(matrizNW, C1, C2, "matrizNW.txt");

    // Convertir a PNG automáticamente
    convertirTxtAPNG("matrizNW.txt", "matrizNW.png");

    // 4. Backtrack
    pair<string, string> resultado = backtrackNW(matrizDir, C1, C2);

    string C1_alineado = resultado.first;
    string C2_alineado = resultado.second;


    // <─────────| Mostrar información |─────────────>

    // | Cadenas alineadas
    cout << "\n  ────────────────────¬"
         << "\n│ Alineamiento óptimo:\n"
         << "│ Cadena 1: " << C1_alineado << "\n"
         << "│ Cadena 2: " << C2_alineado << "\n\n"
         << "  ────────────────────¬" << endl;

    // | Score final del alineamiento
    int scoreFinal = matrizNW[C2.size()][C1.size()];
    cout << "│ ➜  Score\t= " << scoreFinal << "\n";

    // | Matches totales en el alineamiento
    int matches = contarMatches(C1_alineado, C2_alineado);
    cout << "│ ➜  Matches\t= " << matches << "\n";

    // | Mismatches, Gaps y Porcentaje de similitud
    int mismatches, gaps;
    double porcentaje;

    contarMismatchesGapsPorcentaje(C1_alineado, C2_alineado, mismatches, gaps, porcentaje);

    cout << "│ ➜  Mismatches\t= " << mismatches << "\n"
         << "│ ➜  Gaps\t= " << gaps << "\n"
         << "│ ➜  Similitud\t= " << porcentaje << "%\n";

    // Llamada a la función que ahora genera el DOT y el PNG
    generarGraphviz(resultado.first, resultado.second, scoreFinal, "alineamiento_resultado.dot");

    return 0;
}
