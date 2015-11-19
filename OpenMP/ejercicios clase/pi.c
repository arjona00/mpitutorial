/*

This program will numerically compute the integral of

                  4/(1+x*x) 
				  
from 0 to 1.  The value of this integral is pi -- which 
is great since it gives us an easy way to check the answer.

The is the original sequential program.  It uses the timer
from the OpenMP runtime library

History: Written by Tim Mattson, 11/99.

$gcc -fopenmp 

*/
#include <stdio.h>
#include <omp.h>

static long num_steps = 100000000;
double step;
int main ()
{
	  int ID, sum = 0.0;
	  double x, pi;
	  double start_time, run_time;

	  step = 1.0/(double) num_steps;

        	 
	  start_time = omp_get_wtime();

	  #pragma omp parallel num_threads(8) private(ID)
	  {
	  	int id = omp_get_thread_num();
	  	int numThreads = omp_get_num_threads();
	  	double sumHilo = 0.0; //Variable privada de cada hilo


     	for (int i=id; i<= num_steps; i+=numThreads ){
		  x = (i-0.5)*step;
		  sumHilo = sumHilo + 4.0/ (1.0+x*x); //False sharing. Compartición falsa, ya que usan la misma línea de caché para acceder, por lo que ralentiza y pueden solaparse
		}
		#pragma omp atomic
			sum += sumHilo;
		#pragma omp single //Solo se ejecuta una vez, para todos los procesos
			printf("Número de hilos %d\n", numThreads );
	}
		  /*
		  #pragma omp atomic //sin llaves y solo una sentencia, que sea actualización de variable
		  	sum = sum + 4.0/(1.0+x*x); //condicion de carrera. Los hilos leen y escriben a la vez en esta línea
			*/
		  /*
		  #pragma omp critical {
		  	//operaciones criticas tb que deban ser atómicas
		  	//Lo mismo que antes pero más general
		  }
		  */

		  pi = sum * step;
	  run_time = omp_get_wtime() - start_time;
	  printf("\n pi with %ld steps is %lf in %lf seconds\n ",num_steps,pi,run_time);
}	  





