#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
#include "list.h"
#include "nqueue/queue.h"

typedef struct {
  int *best_tour;
  distance_t best_tour_cost;
} tsp_ret_t;

typedef struct {
  int *tour;
  distance_t cost;
  distance_t bound;
  int length;
  int node;
} queue_elem_t;

queue_elem_t *queue_elem_create(int *tour, distance_t cost, distance_t bound, int length, int node) {
  queue_elem_t *elem = (queue_elem_t *) malloc(sizeof(queue_elem_t));
  elem->tour = tour;
  elem->cost = cost;
  elem->bound = bound;
  elem->length = length;
  elem->node = node;
  return elem;
}

void free_queue_elem(queue_elem_t *elem) {
  free(elem);
}

matrix_t *make_matrix_from_file(FILE *fp) {
  int num_cities, num_roads;
  fscanf(fp, "%d %d", &num_cities, &num_roads);

  matrix_t *matrix = matrix_create(num_cities, num_cities, -1);

  for(int i = 0; i < num_roads; i++) {
    int row, column;
    double cost;
    fscanf(fp, "%d %d %lf", &row, &column, &cost);
    matrix_set(matrix, row, column, cost);
    matrix_set(matrix, column, row, cost);
  }
  return matrix;
}

distance_t worst_cost;

void min(matrix_t *distances, int node, distance_t *mins) {
  mins[0] = worst_cost;
  mins[1] = worst_cost;
  for (int i = 0; i < matrix_num_columns(distances); i++) {
    distance_t cost = matrix_get(distances, node, i);
    if (cost < mins[0] && cost > 0) {
      mins[1] = mins[0];
      mins[0] = cost;
    } else if (cost < mins[1] && cost > 0) {
      mins[1] = cost;
    }
  }
}

distance_t initial_lower_bound(matrix_t *distances) {
  distance_t mins[2];
  distance_t lb = 0;
  for (int i = 0; i < matrix_num_columns(distances); i++) {
    min(distances, i, mins);
    lb += mins[0] + mins[1];
  }
  return lb / 2;
}

char cmp_lb(void *queue_elem1, void *queue_elem2) {
  queue_elem_t *elem1 = (queue_elem_t *) queue_elem1;
  queue_elem_t *elem2 = (queue_elem_t *) queue_elem2;
  if (elem1->bound < elem2->bound) {
    return 0;
  } else if (elem1->bound > elem2->bound) {
    return 1;
  } else if (elem1->node < elem2->node) {
    return 0;
  } else {
    return 1;
  }
}

int contains(int *tour, int length, int node) {
  for (int i = 0; i < length; i++) {
    if (tour[i] == node) {
      return 1;
    }
  }
  return 0;
}

distance_t calculate_new_bound(matrix_t *distances, distance_t lb, int f, int t) {
  distance_t mins[2];
  min(distances, f, mins);
  distance_t cf = matrix_get(distances, f, t) >= mins[1] ? mins[1] : mins[0];
  min(distances, t, mins);
  distance_t ct = matrix_get(distances, f, t) >= mins[1] ? mins[1]: mins[0];
  return lb + matrix_get(distances, f, t) - (cf + ct) / 2;
}

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

void print_tour(int *tour, int size) {
  for(int i = 0; i < size; i++) {
    printf("%d ", tour[i]);
  }
  printf("\n");
}

tsp_ret_t tspbb(matrix_t *distances, int N, distance_t best_tour_cost) {
  int *tour = make_tour(N + 1);
  int *best_tour = NULL;
  tour[0] = 0;
  distance_t lb = initial_lower_bound(distances);
  priority_queue_t *queue = queue_create(cmp_lb);
  queue_elem_t *elem = queue_elem_create(tour, 0, lb, 1, 0);
  queue_push(queue, (void *) elem);

  while (queue->size > 0) {
    queue_elem_t *elem = (queue_elem_t *) queue_pop(queue);
    if (elem->bound >= best_tour_cost) {
      tsp_ret_t ret;
      ret.best_tour = best_tour;
      ret.best_tour_cost = best_tour_cost;
      return ret;
    }
    if (elem->length == N && matrix_get(distances, elem->node, 0) != -1) {
      if (elem->cost + matrix_get(distances, elem->node, 0) < best_tour_cost) {
        best_tour = elem->tour;
        best_tour[N] = 0;
        best_tour_cost = elem->cost + matrix_get(distances, elem->node, 0);
      }
    } else {
      for (int v = 0; v < N;  v++) {
        int cost = matrix_get(distances, elem->node, v);
        if (contains(elem->tour, elem->length, v) || cost == -1) {
          continue;
        }
        distance_t new_bound = calculate_new_bound(distances, elem->bound, elem->node, v);
        if (new_bound > best_tour_cost) {
          continue;
        }
        int *new_tour = copy_tour(elem->tour, N + 1);
        new_tour[elem->length] = v;
        distance_t new_cost = elem->cost + matrix_get(distances, elem->node, v);
        queue_elem_t *new_elem = queue_elem_create(new_tour, new_cost, new_bound, elem->length + 1, v);
        queue_push(queue, (void *) new_elem);
      }
    }
  }
  tsp_ret_t ret;
  ret.best_tour = best_tour;
  ret.best_tour_cost = best_tour_cost;
  return ret;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "too few arguments\n");
    fprintf(stderr, "USAGE: ./tsp <matrix.in>\n");
    exit(EXIT_FAILURE);
  }
  distance_t best_tour_cost = atof(argv[2]);
  worst_cost = best_tour_cost;
  FILE *fp = fopen(argv[1], "r");
  matrix_t *distances = make_matrix_from_file(fp);
  tsp_ret_t ret = tspbb(distances, matrix_num_columns(distances), best_tour_cost);
  printf("%lf\n", ret.best_tour_cost);
  print_tour(ret.best_tour, matrix_num_columns(distances) + 1);
  matrix_free(distances);
  fclose(fp);
}