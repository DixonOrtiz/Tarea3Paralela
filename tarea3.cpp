#include <iostream>
#include <vector>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>

using namespace std;

void llenarArregloRandom(vector<int> &);
void mostrarArreglo(vector<int>, int);
void divideArreglo(vector<int>, int, int);


int main(int argc, char **argv){

    int procesador = 0;
    int tamanio = 0;

    vector<int> arreglo;

    llenarArregloRandom(arreglo);


    MPI_Init(NULL, NULL);

    MPI_Comm_rank(MPI_COMM_WORLD, &procesador);
    MPI_Comm_size(MPI_COMM_WORLD, &tamanio);

    divideArreglo(arreglo, tamanio, procesador);


    MPI_Finalize();

    

    return 0;
}

void llenarArregloRandom(vector<int> &arreglo){
    srand(time(NULL));
    int num;

    for(int i = 0; i < 10; i++){
        num=1+rand()%(101-1);
        arreglo.push_back(i);
    }
}

void mostrarArreglo(vector<int> arreglo, int procesador){
    if(procesador == 0){
        for(int i = 0; i < arreglo.size(); i++){
            cout << "Elemento n°" << (i+1) << ": "  << arreglo[i] << endl;
        }
    }
}

void divideArreglo(vector<int> arreglo, int tamanio, int procesador){
    int divisionEntera = (arreglo.size()/tamanio);
    int indiceIzq = 0;
    int indiceDer = 0;

    for(int i = 0; i < tamanio; i++){
        if(procesador == i){
            if(i != (tamanio - 1)){
                int suma = 0;
                indiceIzq = procesador * (divisionEntera);
                indiceDer = indiceIzq + (divisionEntera - 1);

                for(int j = indiceIzq; j <= indiceDer; j++)
                    suma += arreglo[j];
                
                cout << "La suma del procesador " << procesador << " es: " << suma << endl;
                
                
            }

            else{
                int suma = 0;
                indiceIzq = procesador * (divisionEntera);
                indiceDer = (arreglo.size() - 1);

                for(int j = indiceIzq; j <= indiceDer; j++)
                    suma += arreglo[j];
                
                cout << "La suma del procesador " << procesador << " es: " << suma << endl;
                
            }
                
        }
    }
}
