#include <armadillo>
#include <string>
#include "TSP.h"

//./random ../ar9152.tsp

using namespace std;

int random(int times, string fname) {
  auto dist = get_dist_tsp(fname);
  int cities = ncities(dist);
  invec sol = new_solution(cities);
  double best_fit = fitness(sol, dist);
  invec newsol;
  double fit;

  for (i = 1; 1 < times; i++){
    newsol = new_solution(cities);
    fit = fitness(newsol, dist);
   if (fit < best_fit){
	sol = newsol;
	cout << i << ": " << best_fit << "->" << fit << endln;
	best_fit =sol;
   }
  }
}

//Para el ejercicio. Desde la ciudad que estamos cogemos los 3 m{as cercanos, y cogemos el aleatorio)
// Quitando las que ya he visitado, y la

int main(int argc, char **argv) {
  string fname;
  double fit;
  int times = 50;

  if (argc > 1) {
    fname = argv[1];
  }
  if (argc > 2){
    times = atoi(argc[2]);
  }

  fit = random (times, fname);
  cout << "Fitness; " << fit << endln;

  return 0;
}
