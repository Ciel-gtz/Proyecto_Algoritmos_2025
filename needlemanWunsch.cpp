#include <vector> 
#include <fstream>
#include <iostream>

using namespace std;



// ─────────────| Main |─────────────¬

int main(int argc, char** argv) {
	
	// <──| Valores iniciales |──>
	
	string C1, C2, U; 
	int V; 

	/*  ─────────────────────────────
	C1 = Secuencia horizontal | fna
	C2 = Secuencia vertical | fna
	U  = Matriz de puntuación | csv
	V  = Valor de penalización por gap
	    ─────────────────────────────  */


	// <──| Validación de argumentos de entrada |──>

	// | El usuario ingresó menos o más argumentos de los necesarios ➜ sale del programa
	if (argc < 9 || argc > 9) { 
		cout << "⚠️	Faltan o sobran argumentos (DEBEN SER 9 ARGUMENTOS)" 
			 << "\n	Uso: ./needlemanWunsch -C1 [secuenciaH].fna -C2 [secuenciaV].fna -U matrizPuntuacion.csv -V [valor]" 
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
				cout << "⚠️	El valor de penalización por gap (-V) debe ser un número mayor a 0." 
					 << "\n	Valor ingresado ➜ V = '" << V << "'" << endl;
				return 1;
			}
        } 
    }

	// | Algún argumento no tiene valor asignado ➜ sale del programa
	if (C1.empty() || C2.empty() || U.empty()) {
		cout << "⚠️	Algun argumento falta (-C1 = " << C1 << ", -C2 = " << C2 << ", -U = " << U << ", -V = " << V << ")" 
			 << "\n	Uso: ./needlemanWunsch -C1 [secuenciaH].fna -C2 [secuenciaV].fna -U matrizPuntuacion.csv -V [valor]" 
			 << endl;
        return 1;
    }


	// <──| IDK |──>
	
	cout << "hola (todo corrió bien)" << endl;
	
	
	return 0;
}
