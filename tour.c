#include "tour.h"

int *make_tour(int size) {
  int *tour = (int *) malloc(sizeof(int) * size);
  for (int i = 0; i < size; i++) {
    tour[i] = -1;
  }
  return tour;
}

int *copy_tour(int *tour, int size) {
  int *new_tour = make_tour(size);
  for (int i = 0; i < size; i++) {
    new_tour[i] = tour[i];
  }

  return new_tour;
}

int tour_size(int *tour, int size) {
  int i;
  for(i = 0; i < size; i++) {
    if (tour[i] == -1)
      break;
  }
  return i;
}

int contains(int *tour, int length, int node) {
  for (int i = 0; i < length; i++) {
    if (tour[i] == node) {
      return 1;
    }
  }
  return 0;
}

void tour_print(int *tour, int size) {
  for(int i = 0; i < size; i++) {
    printf("%d ", tour[i]);
  }
}