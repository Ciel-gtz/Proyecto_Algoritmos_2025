#include <vector> 
#include <fstream>
#include <iostream>
#include <string> // Para: to_string()
#include <limits> // Para: controlINT()

using namespace std;

// ─────────────| Controles de entrada |─────────────¬

    // | Verificar int del usuario
int controlINT() { 

    int valor;
    while (true){

        cin >> valor;
        if (!cin || valor <= 0){
        cout << "!\tSolo se permiten numeros mayores a 0: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        continue;

        } else {
            return valor;
        }
    }
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
    bool primeraFila = true;   // ← para saltar la fila de encabezados

    while (getline(archivo, linea)) {

        // Si es la primera fila → se ignora COMPLETAMENTE
        if (primeraFila) {
            primeraFila = false;
            continue;
        }

        vector<int> fila;
        string valor;
        bool primeraColumna = true; // ← para saltar la columna de encabezados

        for (char c : linea) {

            if (c == ',') {
                if (primeraColumna) {
                    // Saltar la primera columna (encabezado de fila)
                    primeraColumna = false;
                    valor.clear();
                    continue;
                }

                if (!valor.empty()) {
                    try {
                        int numero = stoi(valor);
                        fila.push_back(numero);
                    } catch (...) {
                        // Valor inválido → ignorar
                    }
                    valor.clear();
                }
            } else {
                valor.push_back(c);
            }
        }

        // Procesar último valor de la fila
        if (!valor.empty() && !primeraColumna) {
            try {
                int numero = stoi(valor);
                fila.push_back(numero);
            } catch (...) {
                // Ignorar si no es válido
            }
        }

        matriz.push_back(fila);
    }

    archivo.close();
    return matriz;
}


// ─────────────| Sobre los archivos |─────────────¬

	// | Corre el bash
int limpiarArchivos(string nombre_archivo, int limite){

	string run = "bash limpiarArchivos.bash " + nombre_archivo + " " + to_string(limite);
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
			 << "\n\tSe utilizará la longitud predeterminada del archivo. Si desea cambiar la longitud, borre del nombre del archivo '_CLEAN-SHORT' y corra este código otra vez." << endl;
		return 2;
	}

	return 0;
}


	// | Actualiza el nombre del archivo en el caso 0 de limpiarArchivos()
string actualizarNombreArchivo(string nombre_archivo) {

	// Remueve '.fna' del nombre de la variable
	string nuevo_nombre = nombre_archivo.substr(0, nombre_archivo.size() - 4);

	// Agrega '_CLEAN-SHORT.fna' a la variable
	return nuevo_nombre + "_CLEAN-SHORT.fna";
}


	// | Guarda la información del archivo en una string
string guardarInfo(string nombre_archivo) {
	string secuencia;
	string linea;

	ifstream archivo(nombre_archivo);

	// Error de lectura
	if (!archivo) {
        cerr << "\n\tNo se pudo abrir el archivo " << nombre_archivo << endl;
        return "Err";
    }

	// Obtiene todas las líneas del archivo y las guarda en una sola string
	while (getline(archivo, linea)) {
		secuencia += linea;
	}

	archivo.close();
	return secuencia;
}

// ─────────────| Main |─────────────¬

int main(int argc, char** argv) {
	
	// <──| Valores iniciales |──>

	string C1, C2, U; 
	int V, limite, valor1, valor2; 

	/*  ─────────────────────────────
	C1 = Secuencia horizontal | fna
	C2 = Secuencia vertical | fna
	U  = Matriz de puntuación | csv
	V  = Valor de penalización por gap
	limite = Longitud máxima de las cadenas
	valor1, valor2 = Valor temporal para validaciones
	    ─────────────────────────────  */


	// <──| Validación de argumentos de entrada |──>

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

			// Verifica que V sea un número mayor a 0
			if ((V == 0) || (V <= 0)) {
                cout << "!\tEl valor de penalización por gap (-V) debe ser mayor a 0.\n";
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


	// <──| Sobre limpiar archivos |──>

	cout << "> Cuál es el máximo de longitud deseado para sus cadenas? : ";
	limite = controlINT();
	
	// | Se limpian archivos con script bash en caso de que no tengan el formato correspondiente (ATCG)
	valor1 = limpiarArchivos(C1, limite);
	valor2 = limpiarArchivos(C2, limite);
	

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


	// <──| Sobre lectura de archivos |──>

	// | C1 y C2 Pasan de ser nombres de archivos a ser las secuencias leídas de estos archivos
	C1 = guardarInfo(C1);
	C2 = guardarInfo(C2);

	// | Error al leer los archivos
	if (C1 == "Err" || C2 == "Err") {
		return 1;
	}
	
	    // ─────────────| Leer y Verificar CSV |─────────────¬
    vector<vector<int>> matrizPuntuacion = leerCSV(U);

    if (matrizPuntuacion.empty()) {
        cerr << "!\tError al cargar la matriz de puntuación.\n";
        return 1;
    }

    cout << "\n> Matriz de puntuación cargada correctamente (" 
        << matrizPuntuacion.size() << " filas):\n";

    for (const vector<int>& fila : matrizPuntuacion) {  // recorre para verificar
        for (int num : fila) {
            cout << num << " "; // imprime la matriz para saber que se lee
        }
        cout << "\n";
    }
    cout << endl;


	return 0;
}
