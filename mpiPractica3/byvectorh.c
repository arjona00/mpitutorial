#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>

#define COLS  12
#define ROWS  8

int main(int argc, char **argv) {

    MPI_Init(&argc, &argv);
    int p, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    char i;

    char a[ROWS*COLS];

    const int BLOCKROWS = ROWS/p;  /* number of rows in _block_ */
    const int BLOCKCOLS = COLS; /* number of cols in _block_ */
    const int NPROWS=p;  /* number of rows in _decomposition_ */
    const int NPCOLS=1;  /* number of cols in _decomposition_ */

    if (rank == 0) {
        for (int ii=0; ii<ROWS*COLS; ii++) {
            a[ii] = (char)ii;
        }
    }

    if (p != NPROWS*NPCOLS) {
        fprintf(stderr,"Error: number of PEs %d != %d x %d\n", p, NPROWS, NPCOLS);
        MPI_Finalize();
        exit(-1);
    }
    char b[BLOCKROWS*BLOCKCOLS];
    for (int ii=0; ii<BLOCKROWS*BLOCKCOLS; ii++) b[ii] = 0;

    MPI_Datatype blocktype;
    MPI_Datatype blocktype2;

    MPI_Type_vector(ROWS/p, COLS, COLS, MPI_CHAR, &blocktype2);
    MPI_Type_create_resized( blocktype2, 0, sizeof(char), &blocktype);
    MPI_Type_commit(&blocktype);

    MPI_Type_vector(ROWS/p+1, COLS, COLS, MPI_CHAR, &blocktyperep2);
    MPI_Type_create_resized( blocktype2, 0, sizeof(char), &blocktyperep);
    MPI_Type_commit(&blocktyperep);



    int disps[NPROWS*NPCOLS];
    int disps_desp[NPROWS*NPCOLS];
    int counts[NPROWS*NPCOLS];
    for (int ii=0; ii<NPROWS; ii++) {
        for (int jj=0; jj<NPCOLS; jj++) {
            disps[ii*NPCOLS+jj] = ii*COLS*BLOCKROWS+jj*BLOCKCOLS;
            disps_desp[ii*NPCOLS+jj] = ii*COLS*BLOCKROWS+jj*BLOCKCOLS-COLS;
            counts [ii*NPCOLS+jj] = 1;
        }
    }

 //   MPI_Scatterv(a, counts, disps, blocktype, b, BLOCKROWS*BLOCKCOLS, MPI_CHAR, 0, MPI_COMM_WORLD);
    MPI_Scatter(a, (BLOCKROWS+1)*BLOCKCOLS, MPI_CHAR, b, 1, blocktyperep, 0, MPI_COMM_WORLD);

    /* each proc prints it's "b" out, in order */
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
        MPI_Barrier(MPI_COMM_WORLD);
    }

    if (rank == 0) {
	memset(a, 0, ROWS*COLS);
        printf("Global matrix again: \n");
                for (int ii=0; ii<ROWS; ii++) {
                    for (int jj=0; jj<COLS; jj++) {
                        printf("%3d ",(int)a[ii*COLS+jj]);
                    }
                    printf("\n");
                }
         
    }

    return 0;

    MPI_Gather(b, BLOCKROWS*BLOCKCOLS, MPI_CHAR, a, BLOCKROWS*BLOCKCOLS, MPI_CHAR, 0, MPI_COMM_WORLD);
//    MPI_Gatherv(b, BLOCKROWS*BLOCKCOLS, MPI_CHAR, a, counts, disps, blocktype, 0, MPI_COMM_WORLD);

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
