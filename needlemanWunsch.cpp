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
        if (!cin || valor == 0){
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
int limpiar_archivos(string filename, int limit){

	string run = "bash " + filename + " " + to_string(limit);
	int status = system(run.c_str());

	if (status == 256) { // 256 es equivalente a exit 1 de bash [256 x exitvalue]
		cout << "!\tError con el archivo " << filename << endl;
		return 1;
	}

	return 0;
}


// ─────────────| Main |─────────────¬

int main(int argc, char** argv) {
	
	// <──| Valores iniciales |──>

	string C1, C2, U; 
	int V, limit; 

	/*  ─────────────────────────────
	C1 = Secuencia horizontal | fna
	C2 = Secuencia vertical | fna
	U  = Matriz de puntuación | csv
	V  = Valor de penalización por gap
	limit = Longitud máxima de las cadenas
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

		// Si el argumento actual [i] es == -C1 ...
        if (actual == "-C1")  {
			// Se guarda el siguiente argumento como C1
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


	// <──| Sobre los archivos |──>

	cout << "> Cuál es el máximo de longitud deseado para sus cadenas? : ";
	limit = controlINT();
	
	// | Se limpian con un script bash en caso de que no tengan el formato correspondiente (ATCG)
	if (limpiar_archivos(C1, limit) == 1 || limpiar_archivos(C2, limit) == 1) {
		return 1;
	}
	
	return 0;
}
