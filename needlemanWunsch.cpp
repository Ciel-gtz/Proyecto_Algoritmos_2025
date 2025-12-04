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
			if ((V == 0) || (V <= 0) || (!V)) {
				cout << "!\tEl valor de penalización por gap (-V) debe ser un número mayor a 0." 
					 << "\n\tValor ingresado ➜ V = '" << V << "'" << endl;
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
	else if (valor1 == 0) {
		actualizarNombreArchivo(C1);
	} else if (valor2 == 0) {
		actualizarNombreArchivo(C2);
	}

	// <──| Sobre lectura de archivos |──>
	
	return 0;
}
