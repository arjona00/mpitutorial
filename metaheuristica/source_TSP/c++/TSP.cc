#include "TSP.h"
#include <fstream>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

vector<string> split(const string &s, char delim) {
  stringstream ss(s);
  string item;
  vector<string> tokens;
  while (getline(ss, item, delim)) {
    tokens.push_back(item);
  }
  return tokens;
}

void _read_positions(string fname, vector<double> &pos_x, vector<double> &pos_y) {
  fstream fs;
  string line;
  bool data;
  int cities, pos;
  
  fs.open(fname, fstream::in);
  string dim_tag = "DIMENSION :";
  string node_tag = "NODE_COORD_SECTION";
  bool data_read = false;

  // Leo las columnas y las guardo en dos vectores x e "y"
  while (getline(fs, line)) {
    if ((pos = line.find(dim_tag)) != string::npos) {
      cities = std::stoi(line.substr(pos+dim_tag.length()));
      pos_x.reserve(cities);
      pos_y.reserve(cities);
    }
    else if ((pos = line.find(node_tag)) != string::npos) {
      data_read = true;
    }
    else if (data_read) {
      if (line.find(" ") != string::npos) {
	auto vars = split(line, ' ');
	pos_x.push_back(stof(vars[1]));
	pos_y.push_back(stof(vars[2]));
      }
    }
    
  }

  fs.close();
}

mat get_dist_tsp(string fname) {
  vector<double> pos_x, pos_y;
  
  _read_positions(fname, pos_x, pos_y);
  // Creo la matriz doble con dichos vectores
  colvec col_x(pos_x);
  colvec col_y(pos_y);
  
  mat positions = join_horiz(col_x, col_y);
  int cities = positions.n_rows;

  mat dist = zeros<mat>(cities, cities);

  for (int i = 0; i < cities-1; i++) {
    auto c1 = positions.row(i);
    
    for (int j=i+1; j < cities; j++) {
      auto c2 = positions.row(j);
      auto distance = sqrt(accu(pow(c1-c2, 2)));
      dist(i, j) = distance;
      dist(j, i) = distance;
    }

  }
	 
  return dist;
}

int ncities(mat dist) {
  return dist.n_rows;
}

double fitness(ivec sol, mat dist) {
  double sum = 0;
  int cities = sol.size();

  for (int i = 0; i < cities-1; i++) {
    auto c1 = sol[i];
    auto c2 = sol[i+1];
    double distance = dist(c1, c2);
    sum += distance;
  }

  auto c1 = sol[cities-1];
  auto c2 = sol[0];
  double distance = dist(c1, c2);
  sum += distance;
  return sum;
}

ivec new_solution(int size) {
  vec range = linspace<vec>(0, size-1, size);
  ivec range_i = conv_to<ivec>::from(range);
  return arma::shuffle(range_i);
}
