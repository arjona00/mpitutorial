#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <mpi.h>

//mpirun -np 4 xterm -e gdb ./mpi_multmatrix

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
  const int SIZE1=4;
  const int SIZE2=4;
  const int SIZE3=4;

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
const int BLOCKROWS_1 = SIZE1/2; //Enviamos  SIZE1/p filas matrix1 por proceso
const int BLOCKCOLS_1 = SIZE2; // Enviamos todas las columnas de matrix1

printf("SIZE1 = %d , SIZE2 = %d , SIZE3 = %d , SIZE1/p = %d , SIZE3/p = %d \n",SIZE1, SIZE2, SIZE3, SIZE1/2, SIZE3/2);

//Para la matriz sub 2
const int BLOCKROWS_2 = SIZE2; //Enviamos todas las filas de matrix2
const int BLOCKCOLS_2 = SIZE3/2; //Enviamos SIZE3/p filas de matrix2 por proceso

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

printf("VECTOR(%d, %d, %d) \n", SIZE1/2, SIZE2, SIZE2 );
MPI_Type_vector(SIZE1/2, SIZE2, SIZE2, MPI_DOUBLE, &blocktype2);
MPI_Type_create_resized( blocktype2, 0, sizeof(double), &blocktype);
MPI_Type_commit(&blocktype);

int NPROWS=p;
int NPCOLS=1;

int disps1[NPROWS*NPCOLS];
int counts1[NPROWS*NPCOLS];


int ii, jj;
/*
for (ii=0; ii<NPROWS; ii++) {
    for (jj=0; jj<NPCOLS; jj++) {
        disps1[ii*NPCOLS+jj] = ii*SIZE2*BLOCKROWS_1+jj*BLOCKCOLS_1;
        counts1 [ii*NPCOLS+jj] = 1;
    }
}

if (rank == 0){
  printf("disps matriz1 en proceso: %d\n",rank);
  ii = 0;
  for (jj=0; jj<NPROWS; jj++) {
      printf("%3d ",(int)disps1[ii*NPROWS+jj]);
  }
  printf("\n");
}
*/

//Enviamos la matriz1 y la submatriz1 donde se almacenan
counts1[0] = 1;
counts1[1] = 1;
counts1[2] = 1;
counts1[3] = 1;
disps1[0] = 0;
disps1[1] = 0;
disps1[2] = 8;
disps1[3] = 8;
MPI_Scatterv(matrix1, counts1, disps1, blocktype, matrix_sub_1, BLOCKROWS_1*BLOCKCOLS_1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

if (rank == 0){
  printf("Matrix1 original en proceso:%d\n",rank);
  print_matrix(SIZE1, SIZE2, matrix1);
}

printf("matrix_sub_1 en proceso:%d\n",rank);
print_matrix(BLOCKROWS_1, BLOCKCOLS_1, matrix_sub_1);



// Preparamos el envío de la matriz 2
MPI_Datatype blocktype3;
MPI_Datatype blocktype4;
printf("VECTOR(%d, %d, %d) \n", SIZE2, SIZE3/2, SIZE3 );
MPI_Type_vector(SIZE2, SIZE3/2, SIZE3, MPI_DOUBLE, &blocktype4);
MPI_Type_create_resized( blocktype4, 0, sizeof(double), &blocktype3);
MPI_Type_commit(&blocktype3);

NPROWS=1;
NPCOLS=p;

int disps2[NPROWS*NPCOLS];
int counts2[NPROWS*NPCOLS];

for (ii=0; ii<NPROWS; ii++) {
    for (jj=0; jj<NPCOLS; jj++) {
        disps2[ii*NPCOLS+jj] = ii*SIZE3*BLOCKROWS_2+jj*BLOCKCOLS_2;
        counts2[ii*NPCOLS+jj] = 1;
    }
}

if (rank == 0){
  printf("disps matriz2 en proceso: %d\n",rank);
  ii = 0;
  for (jj=0; jj<NPCOLS; jj++) {
      printf("%3d ",(int)disps2[ii*NPCOLS+jj]);
  }
  printf("\n");
}

counts2[0] = 1;
counts2[1] = 1;
counts2[2] = 1;
counts2[3] = 1;
disps2[0] = 0;
disps2[1] = 2;
disps2[2] = 0;
disps2[3] = 2;

//Enviamos la matriz2 y la submatriz2 donde se almacenan
MPI_Scatterv(matrix2, counts2, disps2, blocktype3, matrix_sub_2, BLOCKROWS_2*BLOCKCOLS_2, MPI_DOUBLE, 0, MPI_COMM_WORLD);

printf("matrix_sub_2 en proceso:%d\n",rank);
print_matrix(BLOCKROWS_2, BLOCKCOLS_2, matrix_sub_2);



multiply(SIZE1/2, SIZE2, SIZE3/2, matrix_sub_1, matrix_sub_2, matrix_sub_res);

printf("Matrix_sub_res:\n");
print_matrix(SIZE1/2, SIZE3/2, matrix_sub_res);


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

