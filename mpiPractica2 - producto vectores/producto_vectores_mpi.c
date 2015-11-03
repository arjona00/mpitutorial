#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

//Compilacion
//mpicc producto_vectores_mpi.c -o producto_vectores_mpi -lm

//Ejecución
//mpirun -np 20 producto_vectores_mpi

//Realiza el producto de cada elemento del vector y lo inserto en v1
void prod(int *v1, int *v2, int size) {
  int i;

  for (i = 0; i < size; i++) {
    v1[i] = v1[i]*v2[i];
  }
}

//muestra por pantalla los elementos del vector
void printVector(int *v1, int size) {
  int i;

  printf("[");
  for (i = 0; i < size; i++) {
    printf("%d, ",v1[i]);
  }
  printf("]\n");
}

void main(int argc, char **argv) {
   int *vector1;
   int *vector2;
   int size;
   int i;
   //Vectores secundarios para los dos demas procesos
   int *vectormin1, *vectormin2;
   int sizemin;

   //Inicializacion
   MPI_Init(&argc, &argv);
   int rank, nproc;
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Comm_size(MPI_COMM_WORLD, &nproc);

if (rank == 0) {
   size = 10;
   // Reservo memoria
   vector1 = (int *) malloc(size*sizeof(int));
   assert(vector1);
   vector2 = (int *) malloc(size*sizeof(int));
   assert(vector1);


   // Inicio vectores
   for (i=0; i < size; i++) {
       vector1[i] = i+1;
       vector2[i] = size-i;
   }
   printf("Vectores iniciales\n");
   printVector(vector1, size);
   printVector(vector2, size);

}

  //Brodcast lanza el proceso 0 y los demas lo reciben, con una misma llamada
   MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);

   sizemin = size / nproc;
   printf("Sizemin en proceso %d es %d\n", rank, sizemin);

   vectormin1 = (int *) malloc(sizemin*sizeof(int));
   vectormin2 = (int *) malloc(sizemin*sizeof(int));

  //sizemin le mandamos para que sepa en cuanto va a dividir el vector
   MPI_Scatter(vector1, sizemin, MPI_INT, vectormin1, sizemin, MPI_INT, 0, MPI_COMM_WORLD);
   MPI_Scatter(vector2, sizemin, MPI_INT, vectormin2, sizemin, MPI_INT, 0, MPI_COMM_WORLD);

   //Obtengo los resultados de las multiplicaciones parciales, en el vectormin1
   prod(vectormin1,vectormin2, sizemin);
   printf("Producto vectorial parcial en proc %d es \n",rank);
   printVector(vectormin1,sizemin);

    //Cada rama envia al root loss resultados de los vectores parciales en el vector final
    MPI_Gather(vectormin1, sizemin, MPI_INT, vector1, sizemin, MPI_INT, 0,MPI_COMM_WORLD);

if (rank == 0){

   printf("El vector resultado obtenido es: \n");
   printVector(vector1, size);
  }

  if (rank == 0){
     free(vector1);
     free(vector2);
}
     MPI_Finalize();
}
