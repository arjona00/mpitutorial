#include <armadillo>
#include <string>
#include "TSP.h"

using namespace std;

int main(int argc, char **argv) {
  string fname;
  
  if (argc > 1) {
    fname = argv[1];
  }

  auto dist = get_dist_tsp(fname);
  int cities = dist.n_rows;
  cout <<dist;

  ivec sol = new_solution(cities);
  double fit = fitness(sol, dist);
  cout <<"Fitness: " <<fit <<endl;
  
  printf("Hello, world\n");
  return 0;
}
