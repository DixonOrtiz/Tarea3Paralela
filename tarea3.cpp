// Tarea 3 Ayuadantía Dixon Ortiz

#include <iostream>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <iomanip>

using namespace std;

// Declaracion de funciones
void llenarVectorRandom(int vector[], int);
void mostrarVector(int vector[], int);
int* crearSubVector(int vector[], int, int, int);
int calculaSubPromedio(int *subVector, int);
float calcularDesviacion(int vector[], int, float);


int main(int argc, char **argv){

    int procesador = 0;
    int tamanio = 0;
    int vector[100000];

    llenarVectorRandom(vector, 100000);

    MPI_Init(NULL, NULL);

    MPI_Comm_rank(MPI_COMM_WORLD, &procesador);
    MPI_Comm_size(MPI_COMM_WORLD, &tamanio);

    // Procesador maestro
    if(procesador == 0){
        int subPromedioRecibe;
        int sumaSubPromedios = 0;
        float promedioParalelo;
        float desviacion;
        float varianza;
        MPI_Status status;

        // Se imprimme el vector
        mostrarVector(vector, 100000);

        for(int i = 1; i < tamanio; i++){
            MPI_Recv(&subPromedioRecibe, 1, MPI_INT, MPI_ANY_SOURCE, 7, MPI_COMM_WORLD, &status);
            cout << "Subpromedio recibido del " << i << "° proceso: " << subPromedioRecibe << endl;
            sumaSubPromedios += subPromedioRecibe;
        }

        promedioParalelo = sumaSubPromedios/(tamanio-1);
        desviacion = calcularDesviacion(vector, 100000, promedioParalelo);
        varianza = pow(desviacion, 2);
        
        cout << "El promedio del vector es: " << promedioParalelo << endl;
        cout << "La desviación estándar es: " << fixed << setprecision(2) << desviacion << endl;
        cout << "La varianza es: " << fixed << setprecision(2) << varianza << endl;
    }

    // Procesadores esclavos
    else{
        // Esta variable se utiliza para dividir el vector en partes homogéneas entre los procesos
        // exceptuando al último que se lleva una carga distinta ya que equipara la distribución
        // en caso de que ésta no sea exacta
        int divisionEntera = (100000/(tamanio - 1));

        // Se divide el arreglo, luego se calculan los subpromedios y se envián al procesador maestro
        for(int i = 1; i < tamanio; i++){
            // Se trabaja con los procesadores que no son el último
            if(i != (tamanio - 1) && i == procesador){
                int indiceIzq = (procesador - 1) * (divisionEntera);
                int indiceDer = indiceIzq + (divisionEntera - 1);
                int largoSubVector = (indiceDer - indiceIzq) + 1;

                int *subVector = crearSubVector(vector, 100000, tamanio, procesador);
                int subPromedio = calculaSubPromedio(subVector, largoSubVector);

                MPI_Send(&subPromedio, 1, MPI_INT, 0, 7, MPI_COMM_WORLD);

                
            }
            
            // Se trabaja con el último procesador
            else if(i == (tamanio - 1) && i == procesador){
                int indiceIzq = (procesador - 1) * (divisionEntera);
                int indiceDer = 99;
                int largoSubVector = (indiceDer - indiceIzq) + 1;

                int *subVector = crearSubVector(vector, 100000, tamanio, procesador);
                int subPromedio = calculaSubPromedio(subVector, largoSubVector);

                MPI_Send(&subPromedio, 1, MPI_INT, 0, 7, MPI_COMM_WORLD);
            }
        }
    }    
    MPI_Finalize();

    

    return 0;
}

// Definiciones de funciones
void llenarVectorRandom(int vector[], int largo){
    srand(time(NULL));
    int valorRandom;
    
    for(int i = 0; i < largo; i++){
        valorRandom = 1+rand()%(100001-1); 
        vector[i] = valorRandom;
    }
}


void mostrarVector(int vector[], int largo){
    for(int i = 0; i < largo; i++)
        cout << vector[i] << endl;
    cout << "VECTOR DE 100000 VALORES RANDOM ENTRE 1 Y 100000\n" << endl;
}


int* crearSubVector(int vector[], int largo, int tamanio, int procesador){
    int divisionEntera = (largo/(tamanio - 1));
    int indiceIzq = 0;
    int indiceDer = 0;
    int *subVector = NULL;
    int largoSubVector = 0;

    //El procesador actual no es el último
    if(procesador != (tamanio-1)){
        //Se definen los índices iniciales y finales del numero vector
        indiceIzq = (procesador - 1) * (divisionEntera);
        indiceDer = indiceIzq + (divisionEntera - 1);
        largoSubVector = (indiceDer - indiceIzq) + 1;
        
        //Se reserva un espacio de memoria para el subvector
        subVector = new int[largoSubVector];

        //Se llena el subvector
        for(int i = indiceIzq; i <= indiceDer ; i++){
            *subVector = vector[i];
            subVector++;
        }
    }

    //El procesador actual es el último
    else{
        //Se definen los índices iniciales y finales del numero vector
        indiceIzq = (procesador - 1) * (divisionEntera);
        indiceDer = (largo - 1);
        largoSubVector = (indiceDer-indiceIzq) + 1;
        
        //Se reserva un espacio de memoria para el subvector
        subVector = new int[largoSubVector];

        //Se llena el subvector
        for(int i = indiceIzq; i <= indiceDer ; i++){
            *subVector = vector[i];
            subVector++;
        }
    }

    subVector = &vector[indiceIzq];

    return subVector;
}

// Se inngresan los subvectores y se calcula el promedio con punteros
int calculaSubPromedio(int *subVector, int largoSubVector){
    int promedio;
    int sumaSubVector = 0;
    int *punteroRecorre = subVector;

    for(int i = 0; i < largoSubVector; i++){
        sumaSubVector += *punteroRecorre;
        punteroRecorre++;
    }

    promedio = sumaSubVector/largoSubVector;
    
    return promedio;
}

// Se calcula la desviación utilizando el promedio calculado de forma paralela
// en este caso se obvia la función de varianza debido a la nimiedad de su cálculo
float calcularDesviacion(int vector[], int largoVector, float promedioParalelo){
    float sumatoriaCudrados = 0;
    float sumatoriaSobreTotal;
    float desviacion;

    for(int i = 0; i < largoVector; i++)
        sumatoriaCudrados += pow((vector[i] - promedioParalelo), 2);

    sumatoriaSobreTotal = sumatoriaCudrados/(largoVector);
    desviacion = sqrt(sumatoriaSobreTotal);

    return desviacion;
}