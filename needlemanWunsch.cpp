#include <vector> // Para: vector + push_back()
#include <sstream> // Para: stringstream
#include <fstream> // Para: ifstream, ofstream
#include <iostream> // Para: cin, cout, cerr
#include <string> // Para: to_string(), string, getline()
#include <algorithm>  // Para: max, reverse, transform

using namespace std;


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
			 << "\n\tSe utilizará la longitud predeterminada del archivo." 
             << "\n\t!\tSi desea cambiar la longitud: borre del nombre del archivo '_CLEAN-SHORT' y corra este código otra vez.\n" << endl;
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

vector<vector<int>> leerCSV(const string& nombre_archivo) {
    vector<vector<int>> matriz;
    ifstream archivo(nombre_archivo);

    if (!archivo.is_open()) {
        cerr << "!\tNo se pudo abrir el archivo CSV: " << nombre_archivo << endl;
        return matriz;
    }

    string linea;

    // Saltar la primera fila (A,T,C,G)
    getline(archivo, linea);

    while (getline(archivo, linea)) {
        stringstream lineaCSV(linea);
        string casilla;

        vector<int> fila;
        int columnaIdx = 0;

        // Leer cada casilla separada por coma
        while (getline(lineaCSV, casilla, ',')) {
            if (columnaIdx == 0) {
                // Saltar la primera columna ➜ ignorar (A,T,C,G)
                columnaIdx++;
                continue;
            }

            try {
                fila.push_back(stoi(casilla));
                // Esto porque se leen secuencialmente ➜ se quieren agregar secuencialmente ("de arriba para abajo")
            } catch (...) {
                // Captura cualquier excepción de cualquier tipo.
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
    string caracter;

    // Error al abrir el archivo CSV
    if (!file.is_open()) {
        cerr << "!\tNo se pudo abrir el archivo: " << nombreCSV << endl;
        return encabezados;
    }

    // Leer únicamente la primera línea del CSV (la fila de encabezados)
    if (!getline(file, caracter)) {
        // Error al leer la línea
        return encabezados;
    } 

    // Procesar la línea como si fuera un archivo con stringstream para usar getline()
    stringstream ss(caracter);
    string linea;

    // Leer la primera celda (vacía) y descartarla
    getline(ss, caracter, ',');

    // Leer el resto de celdas A, T, C, G (en cualquier orden en el que estén)
    while (getline(ss, caracter, ',')) {
        // Evitar agregar celdas vacías (ej. si hay comas dobles ",,")
        if (!caracter.empty()){
            encabezados.push_back(caracter);
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
int scoreNW(char c1, char c2, const vector<string> &enc, const vector<vector<int>> &mat) {
    int i = -1, j = -1;

    // Buscar el índice del nucleótido c1 y c2 en la lista de encabezados.
    for (int k = 0; k < enc.size(); k++) {
        if (enc[k].size() == 1) {
            if (enc[k][0] == c1) i = k;
            if (enc[k][0] == c2) j = k;
        }
    }

    // Si i o j no se encontraron causará un error de índice.
    if (i == -1 || j == -1) {
        cerr << "!\tNucleótido no encontrado (" << c1 << ", " << c2 << ")\n";
        exit(1); // Salir del programa
    }
    
    return mat[i][j]; // Esto CRASHEA si i o j son -1. Se asume que no lo serán.
}


// | Función para Llenar la Matriz NW 
void llenarMatrizNW(
    const string &C1, const string &C2,
    vector<vector<int>> &matrizNW,
    vector<vector<int>> &matrizDir,
    int V,
    const vector<string> &encabezados,
    const vector<vector<int>> &matPunt
) {
    int filas = matrizNW.size();
    int columnas = matrizNW[0].size();

    for (int i = 1; i < filas; i++) {
        for (int j = 1; j < columnas; j++) {

            int diag = matrizNW[i-1][j-1] + scoreNW(C1[j-1], C2[i-1], encabezados, matPunt);
            int up   = matrizNW[i-1][j]     + V;
            int left = matrizNW[i][j-1]     + V;

            int best = max(diag, max(up, left));
            matrizNW[i][j] = best;

            // Guarda dirección con prioridad diagonal > arriba > izquierda
            if (best == diag)      matrizDir[i][j] = 0;
            else if (best == up)   matrizDir[i][j] = 1;
            else                   matrizDir[i][j] = 2;
        }
    }
}



// | visualización de la matriz NW
void imprimirMatriz(const vector<vector<int>>& matriz, const string& C1, const string& C2) {    
    // Imprimir encabezado de columnas (Secuencia C1)
    cout << "\t\t ─";
    for (char c1 : C1) {
        cout << "\t|" << c1;
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
            cout << "\t|" << matriz[i][j]; 
        }
        cout << "\n";
    }
    cout << endl;
}



/// TOO WEIRD BUT WORKS, should be changed tho
pair<string, string> backtrackNW(const vector<vector<int>>& matrizDir, const string& C1, const string& C2) {
    int i = C2.size();
    int j = C1.size();

    string alineada1;
    string alineada2;

    while (i > 0 || j > 0) {

        // --- Caso borde: primera columna (j == 0) ---
        if (j == 0) {
            alineada1 += '-';
            alineada2 += C2[i - 1];
            i--;
            continue;
        }

        // --- Caso borde: primera fila (i == 0) ---
        if (i == 0) {
            alineada1 += C1[j - 1];
            alineada2 += '-';
            j--;
            continue;
        }

        // Dirección normal desde matrizDir
        int dir = matrizDir[i][j];

        if (dir == 0) {
            // Diagonal
            alineada1 += C1[j - 1];
            alineada2 += C2[i - 1];
            i--; j--;
        }
        else if (dir == 1) {
            // Arriba
            alineada1 += '-';
            alineada2 += C2[i - 1];
            i--;
        }
        else if (dir == 2) {
            // Izquierda
            alineada1 += C1[j - 1];
            alineada2 += '-';
            j--;
        }
        else {
            cerr << "Error: direccion invalida en (" << i << ", " << j << ")\n";
            exit(1);
        }
    }

    // El backtracking construye las cadenas al revés
    reverse(alineada1.begin(), alineada1.end());
    reverse(alineada2.begin(), alineada2.end());

    return {alineada1, alineada2};
}







// ─────────────| Main |─────────────¬

int main(int argc, char** argv) {
	
	// <─────────| Valores iniciales |─────────>

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


	// <─────────| Validación de argumentos de entrada |─────────>

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


	// <─────────| Sobre limpiar archivos |─────────>
	
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


	// <─────────| Sobre lectura de archivos |─────────>

	// | C1 y C2 Pasan de ser nombres de archivos a ser las secuencias leídas de estos archivos
	C1 = guardarInfo(C1);
	C2 = guardarInfo(C2);
	
    cout << "────────────────────────────────────────────"
		 << "\nCadena 1: " << C1 << "\nCadena 2: " << C2 << endl;


   // <─────────| Leer y Verificar CSV |─────────>

    vector<vector<int>> matrizPuntuacion = leerCSV(U);
    vector<string> encabezados = leerEncabezados(U); 

    if (matrizPuntuacion.empty() || encabezados.empty()) {
        cerr << "!\tError al cargar la matriz de puntuación o encabezados.\n";
        return 1;
    }

    // | Impresión de matrizPuntuacion.csv
    cout << "\n\t<──| Matriz de puntuacion (U) |──>\n\t\t|" 
         << nucleotidos[0] << "\t|" << nucleotidos[1] << "\t|" << nucleotidos[2] << "\t|" << nucleotidos[3] << "\n";

    for (const vector<int>& fila : matrizPuntuacion) {  
        cout << "\t| " << nucleotidos[&fila - &matrizPuntuacion[0]]; 

        for (int num : fila) {
            cout << "\t|" << num; 
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
    llenarMatrizNW(C1, C2, matrizNW, matrizDir, V, encabezados, matrizPuntuacion);
    
    // 3. Impresión de la matriz resultante.
	cout << "\n\t\t   -─────────────| Matriz de Programación Dinámica [NW] |─────────────-\n" << endl;
    imprimirMatriz(matrizNW, C1, C2);

    
    // <─────────| Backtrack |─────────>
    //// CHANGE THIS AUTO I DONT LIKE IT
    auto resultado = backtrackNW(matrizDir, C1, C2);


    cout << "\nAlineamiento óptimo:\n";
    cout << resultado.first  << "\n";
    cout << resultado.second << "\n";


    return 0;
}
