#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>

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
   int size = 1000;
   float dist;
   int i;

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

   dist = sqrt(sum(vector1, vector2, size));

   printf("La suma de la distancia euclídea de vector1 y vector2 es : %f\n",
	  dist);

   dist = sqrt(sum(vector1, vector1, size));

   printf("La suma de la distancia euclídea de vector1 y vector1 es : %f\n",
	  dist);

   free(vector1);
   free(vector2);
}
