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
		return 1;
	} else if (status == 512) { // 512 es equivalente a exit 2 de bash
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
        return "Err";
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
    string linea;
    vector<string> enc;

    if (!file.is_open()) {
        cerr << "!\tNo se pudo abrir el archivo de matriz de puntuacion: " << nombreCSV << endl;
        return enc;
    }

    // Leer solo la PRIMERA línea del archivo
    if (getline(file, linea)) {

        size_t inicio = 0;          // Índice de inicio de la búsqueda
        size_t fin = 0;             // Índice de la coma encontrada
        string delimitador = ",";   // La coma como separador
        bool primeraColumna = true;

        // Bucle que busca el delimitador (la coma) en la cadena 'linea'
        while ((fin = linea.find(delimitador, inicio)) != string::npos) {			// esta linea está muy rara (que es npos?)
            
            // Extrae la subcadena entre el 'inicio' y el 'fin' (la coma)
            string valor = linea.substr(inicio, fin - inicio);

            // Saltar la primera columna (celda vacía)
            if (primeraColumna) {
                primeraColumna = false;
                // Mueve el inicio a la posición DERECHA de la coma
                inicio = fin + delimitador.length();
                continue;
            }

            // Si el valor no está vacío, lo agrega
            if (!valor.empty()) {
                enc.push_back(valor);
            }

            // Mueve el inicio a la posición DERECHA de la coma para la siguiente búsqueda
            inicio = fin + delimitador.length();
        }

        // Manejar el ÚLTIMO token que no tiene coma al final (ej. la G final)
        if (inicio < linea.length() && !primeraColumna) {
             string valor = linea.substr(inicio, linea.length() - inicio);
             if (!valor.empty()) enc.push_back(valor);
        }
    }
    return enc;
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

    // Buscar el índice del nucleótido A y B en la lista de encabezados.
    for (int k = 0; k < enc.size(); k++) {
        if (enc[k].size() == 1) {
            if (enc[k][0] == c1) i = k;
            if (enc[k][0] == c2) j = k;
        }
    }

    // Si i o j no se encontraron causará un error de índice. 								aún no hace nada esto*****
    if (i == -1 || j == -1) {
    }
    
    return mat[i][j]; // Esto CRASHEA si i o j son -1. Se asume que no lo serán.
}


// | Función para Llenar la Matriz NW 
void llenarMatrizNW(const string &C1, const string &C2, vector<vector<int>> &matrizNW, int V, const vector<string> &encabezados, const vector<vector<int>> &matPunt) {
    int filas = matrizNW.size();    // tamaño de C2 + 1
    int columnas = matrizNW[0].size(); // tamaño de C1 + 1

    // C1: Horizontal (columnas), C2: Vertical (filas)
    for (int i = 1; i < filas; i++) { // i recorre C2
        for (int j = 1; j < columnas; j++) { // j recorre C1

            // Caso 1: Match/Mismatch (Diagonal)
            // f(i-1, j-1) + U(C1[j-1], C2[i-1])
            int diag = matrizNW[i-1][j-1] + scoreNW(C1[j-1], C2[i-1], encabezados, matPunt);

            // Caso 2: Gap en la secuencia C1 (Movimiento Arriba)
            // f(i-1, j) + V
            int up   = matrizNW[i-1][j] + V; 

            // Caso 3: Gap en la secuencia C2 (Movimiento Izquierda)
            // f(i, j-1) + V
            int left = matrizNW[i][j-1] + V; 

            // El valor f(i, j) es el máximo de las tres opciones.
            matrizNW[i][j] = max(diag, max(up, left));
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
	

	// | Se realizan los casos dependiendo del valor retornado por limpiarArchivos()
	if (valor1 == 1 || valor2 == 1) {
		// Hubo algún error al leer los archivos
		return 1;
	} 
	
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

	// | Error al leer los archivos
	if (C1 == "Err" || C2 == "Err") {
		return 1;
	}
	
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

    // 2. Llenado de la matriz (Programación Dinámica).
    llenarMatrizNW(C1, C2, matrizNW, V, encabezados, matrizPuntuacion);
    
    // 3. Impresión de la matriz resultante.
    // cout << "\n> Puntaje Máximo de Alineamiento (f[n][m]): " << matrizNW[C2.size()][C1.size()] << endl;		➜ no estoy seguro de esto, no sé su uso
	cout << "\n\t\t   -─────────────| Matriz de Programación Dinámica [NW] |─────────────-\n" << endl;
    imprimirMatriz(matrizNW, C1, C2);


    // NOTA: La función de backtracking debe ser implementada aquí para la reconstrucción del alineamiento.

    return 0;
}
