#include <armadillo>
#include <string>
#include "TSP.h"

using namespace std;

int main(int argc, char **argv) {
  string fname = "../test.tsp";
  
  if (argc > 1) {
    fname = argv[1];
  }

  auto dist = get_dist_tsp(fname);
  int cities = dist.n_rows;
  cout <<dist;

  ivec sol_seq = linspace<ivec>(0, cities-1, cities);
  //  cout <<sol_seq;
  ivec sol = new_solution(cities);
  cout <<"Fitness Sequential: " <<fitness(sol_seq, dist) <<endl;

  return 0;
}
