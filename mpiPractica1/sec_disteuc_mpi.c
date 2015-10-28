#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

int sum(int *v1, int *v2, int size) {
  int value, sum = 0;
  int i;

  for (i = 0; i < size; i++) {
    value = v1[i]-v2[i];
    sum += (value*value);
  }

  return sum;
}
 
void main(int argc, char **argv) {
   int *vector1;
   int *vector2;
   int size;
   float dist;
   int i;
   //Vectores secundarios para los dos demas procesos
   int *vectormin1, *vectormin2;
   int sizemin;
   int sumaparcial;
   int sumatotal;

   //Inicializacion
   MPI_Init(&argc, &argv);
   int rank, nproc;
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Comm_size(MPI_COMM_WORLD, &nproc);

if (rank == 0) {
   size = 1000;
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
}
  //Brodcast lanza el proceso 0 y los demas lo reciben, con una misma llamada
   MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);

   sizemin = size / nproc;
   printf("Sizemin en proceso %d es %d\n", rank, sizemin);

   vectormin1 = (int *) malloc(sizemin*sizeof(int));
   vectormin2 = (int *) malloc(sizemin*sizeof(int));

  //sizemin le mandamos para que sepa en cuanto va a dividir el vector
   MPI_Scatter(vector1, sizemin, MPI_INT, vectormin1, sizemin, MPI_INT, 0,MPI_COMM_WORLD);
   MPI_Scatter(vector2, sizemin, MPI_INT, vectormin2, sizemin, MPI_INT, 0,MPI_COMM_WORLD);

   sumaparcial = sum(vectormin1,vectormin2, sizemin);

   //Realiza el map (todos las sumas parciales) a reduce, en la suma total
   // del proceso 0, que es el que obtiene el valor
   MPI_Reduce(&sumaparcial, &sumatotal, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

if (rank == 0){

   dist = sqrt(sumatotal);

   printf("La suma de la distancia euclídea de vector1 y vector2 es : %f\n",
	  dist);
/*
   dist = sqrt(sum(vector1, vector1, size));

   printf("La suma de la distancia euclídea de vector1 y vector1 es : %f\n",
	  dist);
*/
  }

  if (rank == 0){
     free(vector1);
     free(vector2);
}
     MPI_Finalize();
}
