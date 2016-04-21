#!/usr/bin/python3
import pandas as pd
import numpy as np
from scipy.spatial.distance import pdist, squareform

def get_dist_tsp(fname='ar9152.tsp'):
    """
    Returns the list with the positions of the cities.

    Parameters
    ----------
    fname -- filename with the TSP
    """
    # Use pandas to get the positions
    df = pd.read_table(fname, sep=' ', skiprows=7, skipfooter=1,
                       names=['x', 'y'], engine='python')
    # Normalize positions
    #df = (df-df.min())/df.max()
    # Calculate distance
    dist = squareform(pdist(df))
    return dist


def fitness_better(solution, cost):
    """
    Return the fitness evaluation. It is the medium implementation.

    Parameters
    ----------
    solution -- Vector of the cities.
    cost -- distance matrix.

    return -- the final fitness.
    """
    solution_rot = np.roll(solution, -1)
    return np.sum([cost[pair] for pair in zip(solution, solution_rot)])


def fitness(solution, cost):
    """
    Return the fitness evaluation. It is the quicker implementation.

    Time for 1000 cities: 1.5ms

    Parameters
    ----------
    solution -- Vector of the cities.
    cost -- distance matrix.

    return -- the final fitness.
    """
    solution_rot = np.roll(solution, -1)
    return np.sum(cost[solution, solution_rot])


def ncities(cost):
    """
    Return the number of cities
    """
    nc, _ = cost.shape
    return nc


def fitness_slow(solution, cost):
    """
    Return the fitness evaluation. It is the lower implementation

    Time for 1000 cities: 17ms

    Parameters
    ----------
    solution -- Vector of the cities.
    cost -- distance matrix.

    return -- the final fitness
    """
    sum = 0
    num_ciudades = len(solution)

    for i in range(num_ciudades-1):
        c1 = solution[i]
        c2 = solution[i+1]
        sum += cost[c1,c2]
    # Volver a la ciudad inicial
    ciudad_final = solution[num_ciudades-1]
    ciudad_origen = solution[0]
    sum += cost[ciudad_final, ciudad_origen]
    return sum
