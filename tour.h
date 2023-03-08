#ifndef _TSP_TOUR_H
#define _TSP_TOUR_H

#include "stdio.h"
#include "stdlib.h"

int *make_tour(int size);
int *copy_tour(int *tour, int size);
int tour_size(int *tour, int size);
int contains(int *tour, int length, int node);
void tour_print(int *tour, int size);

#endif