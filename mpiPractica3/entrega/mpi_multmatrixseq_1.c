#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <mpi.h>

void print_matrix(int size1, int size2, double *matrix) {
  int i, j;

  for (i = 0; i < size1; i++) {
    for (j = 0; j < size2; j++) {
      printf("%.0lf ", matrix[i*size2+j]);
    }

    printf("\n");
  }


}

void multiply(int size1, int size2, int size3,
              double *matrix1, double *matrix2,
              double *result) {
  int i, j, k;
  
  for (k = 0; k < size3; k++) {
    for (i = 0; i < size1; i++) {
      double sum = 0;

      for (j = 0; j < size2; j++) {
        sum += matrix1[i*size2+j]*matrix2[j*size3+k];
      }

      result[i*size3+k] = sum;
    }

  }
  
}

double *new_matrix(int size1, int size2) {
  double *m = (double *)malloc(size1*size2*sizeof(double));
  assert (m != NULL);
  return m;
}

void free_matrix(double *matrix) {
  if (matrix != NULL) {
    free(matrix);
  }
}

int main(int argc, char *argv[]) {

  MPI_Init(&argc, &argv);
  int p, rank;
  MPI_Comm_size(MPI_COMM_WORLD, &p);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  double *matrix1;
  double *matrix2;
  double *result;
  const int SIZE1=3;
  const int SIZE2=6;
  const int SIZE3=3;

  int i, j;

  matrix1 = new_matrix(SIZE1, SIZE2);
  matrix2 = new_matrix(SIZE2, SIZE3);
  result = new_matrix(SIZE1, SIZE3);

  // Init matrixes
  for(i = 0; i < SIZE1; i++) {
    for (j = 0; j < SIZE2; j++) {
      matrix1[i*SIZE2+j] = i+1;
    }
  }
  
  for(i = 0; i < SIZE2; i++) {
    for (j = 0; j < SIZE3; j++) {
      matrix2[i*SIZE3+j] = j+1;
    }
  }
  if (rank == 0){
    printf("Matrix1:\n");
    print_matrix(SIZE1, SIZE2, matrix1);
    printf("Matrix2:\n");
    print_matrix(SIZE2, SIZE3, matrix2);
    
    // Apply the matrix multiplication
    multiply(SIZE1, SIZE2, SIZE3, matrix1, matrix2, result);
    printf("Matrix result sequential:\n");
    print_matrix(SIZE1, SIZE3, result);
  }

// Comienzo de procesamiento mpi

    

printf("Proceso %d de %d\n",rank,p);

//Creamos las matrices reducidas
//Inicialización de matrices: Matriz Fila de A, Matriz Columna de B, y resultado para cada proceso
double *matrix_sub_1;
double *matrix_sub_2;
double *matrix_sub_res;

//Para la matriz sub 1
const int BLOCKROWS_1 = SIZE1; //SIZE1/p; //Enviamos  x filas matrix1 por proceso
const int BLOCKCOLS_1 = SIZE2/p; // Enviamos todas las columnas de matrix1

//Para la matriz sub 2
const int BLOCKROWS_2 = SIZE2; //Enviamos todas las filas de matrix2
const int BLOCKCOLS_2 = 1; //SIZE3/p; //Enviamos x filas de matrix2 por proceso

matrix_sub_1 = new_matrix(BLOCKROWS_1, BLOCKCOLS_1);
matrix_sub_2 = new_matrix(BLOCKROWS_2, BLOCKCOLS_2);
matrix_sub_res = new_matrix(BLOCKROWS_1, BLOCKCOLS_2);

for (i = 0; i < BLOCKROWS_1; i++) {
  for (j = 0; j < BLOCKCOLS_1; j++) {
    matrix_sub_1[i*BLOCKCOLS_1+j] = 0;
  }
}

for (i = 0; i < BLOCKROWS_2; i++) {
  for (j = 0; j < BLOCKCOLS_2; j++) {
    matrix_sub_2[i*BLOCKCOLS_2+j] = 0;
  }
}

for (i = 0; i < BLOCKROWS_1; i++) {
  for (j = 0; j < BLOCKCOLS_2; j++) {
    matrix_sub_res[i*BLOCKCOLS_2+j] = 0;
  }
}

// Preparamos el envío de la matriz 1
MPI_Datatype blocktype;
MPI_Datatype blocktype2;

MPI_Type_vector(SIZE1, SIZE2/p, SIZE2, MPI_DOUBLE, &blocktype2);
MPI_Type_create_resized( blocktype2, 0, sizeof(double), &blocktype);
MPI_Type_commit(&blocktype);

int NPROWS=1;
int NPCOLS=p;

int disps[NPROWS*NPCOLS];
int counts[NPROWS*NPCOLS];
int ii, jj;
for (ii=0; ii<NPROWS; ii++) {
    for (jj=0; jj<NPCOLS; jj++) {
        disps[ii*NPCOLS+jj] = ii*SIZE2*BLOCKROWS_1+jj*BLOCKCOLS_1;
        counts [ii*NPCOLS+jj] = 1;
    }
}

if (rank == 0){
  printf("disps en proceso: %d\n",rank);
  ii = 0;
  for (jj=0; jj<NPCOLS; jj++) {
      printf("%3d ",(int)disps[ii*NPCOLS+jj]);
  }
  printf("\n");
  
  printf("matrix_sub_1 en proceso: %d\n",rank);
  print_matrix(BLOCKROWS_1, BLOCKCOLS_1, matrix_sub_1);
}


MPI_Scatterv(matrix1, counts, disps, blocktype, matrix_sub_1, BLOCKROWS_1*BLOCKCOLS_1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

printf("Matrix1 en proceso:%d\n",rank);
print_matrix(SIZE1, SIZE2, matrix1);

printf("matrix_sub_1 en proceso:%d\n",rank);
print_matrix(BLOCKROWS_1, BLOCKCOLS_1, matrix_sub_1);


/*
multiply(1, SIZE2, 1, matrix_sub_1, matrix_sub_2, matrix_sub_res);

printf("Matrix_sub_res:\n");
print_matrix(1, SIZE2, matrix_sub_res);
*/
//Recuperamos los resultados






  /*
  printf("Matrix1:\n");
  print_matrix(SIZE1, SIZE2, matrix1);
  printf("Matrix2:\n");
  print_matrix(SIZE2, SIZE3, matrix2);
  
  // Apply the matrix multiplication
  multiply(SIZE1, SIZE2, SIZE3, matrix1, matrix2, result);
  printf("Matrix3:\n");
  print_matrix(SIZE1, SIZE3, result);
*/
  // Free the matrix
  free_matrix(matrix_sub_1);
  free_matrix(matrix_sub_2);
  free_matrix(matrix_sub_res);

  free_matrix(matrix1);
  free_matrix(matrix2);
  free_matrix(result);
  
  MPI_Finalize();

  return 0;
}

