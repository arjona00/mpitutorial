#include <stdio.h>
#include <stdlib.h>

#define SIZE1 3
#define SIZE2 2
#define SIZE3 4

void print_matrix(int size1, int size2, double matrix[size1][size2]) {
  int i, j;

  for (i = 0; i < size1; i++) {
    for (j = 0; j < size2; j++) {
      printf("%.0lf ", matrix[i][j]);
    }

    printf("\n");
  }


}

void multiply(int size1, int size2, int size3,
              double matrix1[size1][size2], double matrix2[size2][size3],
              double result[size1][size3]) {
  int i, j, k;
  
  for (k = 0; k < size3; k++) {
    for (i = 0; i < size1; i++) {
      double sum = 0;

      for (j = 0; j < size2; j++) {
        sum += matrix1[i][j]*matrix2[j][k];
      }

      result[i][k] = sum;
    }

  }
  
}

int main(int argc, char *argv[]) {
  double matrix1[SIZE1][SIZE2];
  double matrix2[SIZE2][SIZE3];
  double result[SIZE1][SIZE3];
  int i, j;


  // Init matrixes
  for(i = 0; i < SIZE1; i++) {
    for (j = 0; j < SIZE2; j++) {
      matrix1[i][j] = i;
    }
  }
  
  for(i = 0; i < SIZE2; i++) {
    for (j = 0; j < SIZE3; j++) {
      matrix2[i][j] = j;
    }
  }

  printf("Matrix1:\n");
  print_matrix(SIZE1, SIZE2, matrix1);
  printf("Matrix2:\n");
  print_matrix(SIZE2, SIZE3, matrix2);
  
  // Apply the matrix multiplication
  multiply(SIZE1, SIZE2, SIZE3, matrix1, matrix2, result);
  printf("Matrix3:\n");
  print_matrix(SIZE1, SIZE3, result);
  
  return 0;
}

