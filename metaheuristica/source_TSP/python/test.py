import TSP
import numpy as np
import sys

def main(fname):
    fname = "../test.tsp"
    dist = TSP.get_dist_tsp(fname)
    nc = TSP.ncities(dist)
    sol = np.arange(nc)
    
    print(dist)
    best = TSP.fitness(sol, dist)
    print("Sequential: {}".format(best))
    # sol, best = search_random(times, dist)
    # print("Random Search: {}".format(best))
    # sol, best = greedy(dist)
    # print("Greedy Search: {}".format(best))
    # sol, best = grasp(times, dist, nelem=2)
    # print("GRASP Search: {}".format(best))
    
if __name__ == '__main__':
    if len(sys.argv)>1:
        fname = sys.argv[1]
    else:
        fname = None
        
    main(fname)
