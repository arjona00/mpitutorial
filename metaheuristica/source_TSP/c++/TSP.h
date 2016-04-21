#ifndef TSP_H
#define TSP_H

#include <string>
#include <armadillo>

using namespace arma;
using namespace std;

mat get_dist_tsp(string fname);
double fitness(ivec sol, mat dist);
ivec new_solution(int size);
int ncities(mat dist);


#endif /* TSP_H */
