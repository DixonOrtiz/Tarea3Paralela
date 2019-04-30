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
    int divisionEntera = (arreglo.size()/(tamanio - 1));
    int indiceIzq = 0;
    int indiceDer = 0;

    if(procesador == 0){
            int sumatotal = 0;
            int recibeInfo = 0;
            MPI_Status status;

            for(int i = 1; i < tamanio; i++){
                MPI_Recv(&recibeInfo, 1, MPI_INT, i, 7, MPI_COMM_WORLD, &status);
                cout << "La suma recibida del procesador " << i << " es: " << recibeInfo << endl;

            }
    }

    for(int i = 1; i < tamanio; i++){

        if(procesador == i){
            int suma = 0;
            if(i != (tamanio - 1)){
                
                
                indiceIzq = (procesador - 1) * (divisionEntera);
                indiceDer = indiceIzq + (divisionEntera - 1);

                for(int j = indiceIzq; j <= indiceDer; j++)
                    suma += arreglo[j];
            }

            else{
                
                indiceIzq = (procesador - 1) * (divisionEntera);
                indiceDer = (arreglo.size() - 1);

                for(int j = indiceIzq; j <= indiceDer; j++)
                    suma += arreglo[j];                     
            }

            MPI_Send(&suma, 1, MPI_INT, 0, 7, MPI_COMM_WORLD);

        }      
    }
}
