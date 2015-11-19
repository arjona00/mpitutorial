#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>


//Multiplicación de dos matrices, AxB = C
//Matriz A
#define ROWS_A  3
#define COLS_A  4

//Matriz B
#define ROWS_B  4
#define COLS_B  3

//Matriz C, dimensiones A filas, B columnas
#define ROWS_C  ROWS_A
#define COLS_C  COLS_B



// compila mpic++ para que no de fallos de grupos
// mpic++ byvectorv -o byvectorv 
// mpirun -np 10 byvectorv

/* División de columnas verticales */

int main(int argc, char **argv) {

    MPI_Init(&argc, &argv);
    int p, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);    

    int a[ROWS_A*COLS_A];
    int B[ROWS_B*COLS_B];
    int C[ROWS_C*COLS_C];

    const int BLOCKROWS = ROWS_C;  /* number of rows in _block_ */
    const int BLOCKCOLS = COLS_C/p; /* number of cols in _block_ */ /* Puede provocar que no se realice algún cálculo. Hay dos formas de enfocarlo. Que uno calcule más,o recalcular con un proceso menos para que sea par */
    const int NPROWS=1;  /* number of rows in _decomposition_ */ /* por nuestro enfoque, cada proceso devuelve una fila */
    const int NPCOLS=p;  /* number of cols in _decomposition_ */ /* por nuestro enfoque, cada proceso devuelve columnas igual al número de procesos */

    if (rank == 0) {
    	/* Relleno */
        for (int ii=0; ii<ROWS_A*COLS_A; ii++) {
            a[ii] = ii;
        }

        for (int ii=0; ii<ROWS_B*COLS_B; ii++) {
            b[ii] = ii;
        }
    }

	/* Controla que la multiplicación de matrices cumple la regla  */
    if (COLS_A != ROWS_B) {
        fprintf(stderr,"Error: number of array dimension %d != %d ", COLS_A, ROWS_B);
        MPI_Finalize();
        exit(-1);
    }

    /* Controla que el numero de procesos coincide con los datos a procesar */
    if (p != NPROWS*NPCOLS) {
        fprintf(stderr,"Error: number of PEs %d != %d x %d\n", p, NPROWS, NPCOLS);
        MPI_Finalize();
        exit(-1);
    }

    /* Para cada proceso, donde guarda los datos recibidos */
    int r[BLOCKROWS*BLOCKCOLS]; //3 x ( 3 / p )
    for (int ii=0; ii<BLOCKROWS*BLOCKCOLS; ii++) r[ii] = 0; //inicialización vector que usa cada proceso

    MPI_Datatype blocktype;
    MPI_Datatype blocktype2;

    //Primer vector A
    MPI_Type_vector(
    	COLS_A, //Número de elementos, que corresponden con el tamaño de la fila
    	ROWS_A/p, //tamaño de cada uno
    	COLS_A, //offset, desplazamiento con el siguiente
    	MPI_INT, 
    	&blocktype2);

    MPI_Type_create_resized( //Por ahora sin explicación, ya lo dirá Daniel
    	blocktype2, 
    	0, 
    	sizeof(int), 
    	&blocktype);

    MPI_Type_commit(&blocktype);

    int disps[NPROWS*NPCOLS]; //Los organizamos en una sola fila
    int counts[NPROWS*NPCOLS]; 
    for (int ii=0; ii<NPROWS; ii++) {
        for (int jj=0; jj<NPCOLS; jj++) {
            disps[ii*NPCOLS+jj] = ii*COLS*BLOCKROWS+jj*BLOCKCOLS; //Mismo q en la transparencia, todos del mismo tamaño
            counts [ii*NPCOLS+jj] = 1; //Esto puede complicarse, si se incluye condiciones de borde en algunos procesos
        }
    }

    MPI_Scatterv(
    	a, //Matriz origen
    	counts,  //numero de elementos de cada proceso
    	disps, //Desplazamientos
    	blocktype, 
    	r, //Donde se acumulan los datos recibidos
    	BLOCKROWS*BLOCKCOLS, //Tamaño del vector b
    	MPI_INT, 
    	0, //proceso root
    	MPI_COMM_WORLD);

    /* each proc prints it's "b" out, in order */ //Esto se hace así para que sea en orden, solo lo hace cada proceso si le toca. En procedimientos no ordenados, no hace falta
    for (int proc=0; proc<p; proc++) {
        if (proc == rank) {
            printf("Rank = %d\n", rank);
            if (rank == 0) {
                printf("Global matrix: \n");
                for (int ii=0; ii<ROWS; ii++) {
                    for (int jj=0; jj<COLS; jj++) {
                        printf("%3d ",(int)a[ii*COLS+jj]);
                    }
                    printf("\n");
                }
            }
            printf("Local Matrix:\n");
            for (int ii=0; ii<BLOCKROWS; ii++) {
                for (int jj=0; jj<BLOCKCOLS; jj++) {
                    printf("%3d ",(int)b[ii*BLOCKCOLS+jj]);
                }
                printf("\n");
            }
            printf("\n");
        }
        MPI_Barrier(MPI_COMM_WORLD); //todos los procesos deben llegar hasta aqui para poder continuar
    }

    if (rank == 0) {
		memset(a, 0, ROWS*COLS); //Borra la matriz original a 0
        printf("Global matrix again: \n");
                for (int ii=0; ii<ROWS; ii++) {
                    for (int jj=0; jj<COLS; jj++) {
                        printf("%3d ",(int)a[ii*COLS+jj]);
                    }
                    printf("\n");
                }
         
    }

    MPI_Gatherv(b, BLOCKROWS*BLOCKCOLS, MPI_CHAR, a, counts, disps, blocktype, 0, MPI_COMM_WORLD);

        if (rank == 0) {
                printf("Global matrix again: \n");
                for (int ii=0; ii<ROWS; ii++) {
                    for (int jj=0; jj<COLS; jj++) {
                        printf("%3d ",(int)a[ii*COLS+jj]);
                    }
                    printf("\n");
                }
            }
     

    MPI_Finalize();

    return 0;
}
