#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
#include "list.h"

typedef struct {
  list_t *best_tour;
  int best_tour_cost;
} tsp_ret_t;

typedef struct {
  list_t *tour;
  int cost;
  int bound;
  int length;
  int node;
} queue_elem_t;

matrix_t *make_matrix_from_file(FILE *fp) {
  int num_cities, num_roads;
  fscanf(fp, "%d %d", &num_cities, &num_roads);

  matrix_t *matrix = matrix_create(num_cities, num_cities, -1);

  for(int i = 0; i < num_roads; i++) {
    int row, column, cost;
    fscanf(fp, "%d %d %d", &row, &column, &cost);
    matrix_set(matrix, row, column, cost);
  }
  return matrix;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "too few arguments\n");
    fprintf(stderr, "USAGE: ./tsp <matrix.in>\n");
    exit(EXIT_FAILURE);
  }
  FILE *fp = fopen(argv[1], "r");
  matrix_t *matrix = make_matrix_from_file(fp);
  matrix_print(matrix);
  matrix_free(matrix);
  fclose(fp);
}