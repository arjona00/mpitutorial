// Version tuneada por Antonio Arjona
// Author: Wes Kendall
// Copyright 2011 www.mpitutorial.com
// This code is provided freely with the tutorials on mpitutorial.com. Feel
// free to modify it for your own use. Any distribution of the code must
// either provide a link to www.mpitutorial.com or keep this header in tact.
//
// Ping pong example with MPI_Send and MPI_Recv. Two processes ping pong a
// number back and forth, incrementing it until it reaches a given value.
//
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
  const int PING_PONG_LIMIT = 10;

  // Initialize the MPI environment
  MPI_Init(NULL, NULL);
  // Find out rank, size
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  // We are assuming at least 2 processes for this task
  if (world_size < 2) {
    fprintf(stderr, "World size must be more than one for %s\n", argv[0]);
    MPI_Abort(MPI_COMM_WORLD, 1); 
  }

int part_rank; //Rank del proceso que ha enviado el mensaje

  //Proceso 0 inicializa
  if (world_rank != 0){
    int part_rank = 0;

    MPI_Recv(
	&part_rank,
	1, MPI_INT,
	world_rank -1,
	0,
	MPI_COMM_WORLD,
	MPI_STATUS_IGNORE);

    char proc_name[MPI_MAX_PROCESSOR_NAME];
    int len;
    MPI_Get_processor_name(proc_name, &len);
    printf("Proceso %d en %s, ha recibido mensaje del proceso %d con valor %d\n",
           world_rank, proc_name, world_rank -1, part_rank);
  }
  //Todos los procesos envian mensaje al siguiente proceso con su rank
  MPI_Send(
	&world_rank,
	1, MPI_INT,
	(world_rank+1) % world_size, //ultimo envia al primero
	0,
	MPI_COMM_WORLD);

  if (world_rank == 0){
    //El proceso 0 finaliza las comunicaciones cuando recibe del ultimo
    MPI_Recv(
	&part_rank,
	1, MPI_INT,
	world_size - 1, //El proceso 0 recibe del ultimo proceso
	0,
	MPI_COMM_WORLD,
        MPI_STATUS_IGNORE);

    printf("Proceso %d ha recibido %d del proceso %d y finaliza\n",
           world_rank, part_rank, world_size -1);
  }
  MPI_Finalize();
}
