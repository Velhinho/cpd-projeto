#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
#include "list.h"
#include "nqueue/queue.h"
#include <omp.h>
#include <mpi.h>

#define NUM_ELEMS 10
#define RECV_RET_TAG 1
#define RECV_TOUR_TAG 2

typedef struct {
  int *best_tour;
  distance_t best_tour_cost;
  int count;
} tsp_ret_t;

typedef struct {
  list_t *tour;
  distance_t cost;
  distance_t bound;
  int length;
  int node;
  long rtz;
  long contains;
} queue_elem_t;

queue_elem_t *queue_elem_create(
    list_t *tour, distance_t cost, distance_t bound, int length, int node, long rtz, long contains) {
  queue_elem_t *elem = (queue_elem_t *) malloc(sizeof(queue_elem_t));
  elem->tour = tour;
  elem->cost = cost;
  elem->bound = bound;
  elem->length = length;
  elem->node = node;
  elem->rtz = rtz;
  elem->contains = contains;
  return elem;
}

void queue_elem_print(queue_elem_t *elem) {
  printf("queue_elem(tour ");
  list_print(elem->tour);
  printf(", cost %lf, bound %lf, length %d, node %d)", elem->cost, elem->bound, elem->length, elem->node);
  printf("\n");
}

void queue_elem_free(queue_elem_t *elem) {
  list_free(elem->tour);
  free(elem);
}

matrix_t *make_matrix_from_file(FILE *fp) {
  int num_cities, num_roads;
  int n = fscanf(fp, "%d %d", &num_cities, &num_roads);
  if (n < 2) {
    fprintf(stderr, "error reading file\n");
    exit(-1);
  }

  matrix_t *matrix = matrix_create(num_cities, num_cities, -1);

  for(int i = 0; i < num_roads; i++) {
    int row, column;
    double cost;
    int n = fscanf(fp, "%d %d %lf", &row, &column, &cost);
    if (n < 3) {
      fprintf(stderr, "error reading file\n");
      exit(-1);
    }
    matrix_set(matrix, row, column, cost);
    matrix_set(matrix, column, row, cost);
  }
  return matrix;
}

distance_t worst_cost;
matrix_t *mins;

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
  int num_columns = matrix_num_columns(distances);
  mins = matrix_create(num_columns, 2, -1);
  for (int i = 0; i < num_columns; i++) {
    min(distances, i, mins->matrix_elements[i]);
  }

  distance_t lb = 0;
  for (int i = 0; i < num_columns; i++) {
    lb += matrix_get(mins, i, 0) + matrix_get(mins, i, 1);
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

distance_t get_min(distance_t lb, int node) {
  return lb >= matrix_get(mins, node, 1) ? matrix_get(mins, node, 1) : matrix_get(mins, node, 0);
}

distance_t calculate_new_bound(matrix_t *distances, distance_t lb, int f, int t) {
  distance_t cf = get_min(matrix_get(distances, f, t), f);
  distance_t ct = get_min(matrix_get(distances, f, t), t);
  return lb + matrix_get(distances, f, t) - (cf + ct) / 2;
}

void queue_delete_all(priority_queue_t *queue) {
  while (queue->size != 0) {
    queue_elem_t *elem = (queue_elem_t *) queue_pop(queue);
    queue_elem_free(elem);
  }
  queue_delete(queue);
}

long get_bit(long bits, long index) {
  long mask = 1 << index;
  return bits & mask;
}

long set_bit(long bits, long index) {
  long mask = 1 << index;
  return bits | mask;
}

long rtz_init(matrix_t *distances) {
  long rtz = 0;
  int num_columns = matrix_num_columns(distances);
  for (int i = 0; i < num_columns; i++) {
    double elem = matrix_get(distances, 0, i);
    if (elem != -1) {
      rtz = set_bit(rtz, i);
    }
  }
  return rtz;
}


int pid, num_procs;

priority_queue_t *discard(priority_queue_t *queue) {
  priority_queue_t *new_queue = queue_create(cmp_lb);
  int i = 0;
  while (new_queue->size < NUM_ELEMS) {
    queue_elem_t *elem = queue_pop(queue);
    if (i % num_procs == pid) {
      queue_push(new_queue, elem);
    }
    i++;
  }
  return new_queue;
}

tsp_ret_t tspbb(matrix_t *distances, int N, distance_t best_tour_cost) {
  list_t *tour = list_singleton(0);
  list_t *best_tour = list_empty();
  long rtz = rtz_init(distances);

  distance_t lb = initial_lower_bound(distances);
  priority_queue_t *queue = queue_create(cmp_lb);
  queue_elem_t *elem = queue_elem_create(tour, 0, lb, 1, 0, rtz, 1);
  queue_push(queue, (void *) elem);

  while (queue->size > 0 && queue->size < (size_t) NUM_ELEMS * num_procs) {
    queue_elem_t *elem = (queue_elem_t *) queue_pop(queue);
    //queue_elem_print(elem);
    //printf("\n");
    if (elem->bound >= best_tour_cost) {
      int * best_tour_ret = (int *) malloc(sizeof(int) * (N + 1));
      memcpy(best_tour_ret, best_tour->elements, sizeof(int) * best_tour->count);
      tsp_ret_t ret = { best_tour_ret, best_tour_cost, best_tour->count };
      queue_elem_free(elem);
      list_free(best_tour);
      return ret;
    }
    if (elem->length == N && matrix_get(distances, elem->node, 0) != -1) {
      if (elem->cost + matrix_get(distances, elem->node, 0) < best_tour_cost) {
        list_free(best_tour);
        best_tour = list_append(elem->tour, 0);
        best_tour_cost = elem->cost + matrix_get(distances, elem->node, 0);
      }
    } else {
      for (int v = 0; v < N;  v++) {
        int cost = matrix_get(distances, elem->node, v);
        if (get_bit(elem->contains, v) || cost == -1) {
          continue;
        }
        distance_t new_bound = calculate_new_bound(distances, elem->bound, elem->node, v);
        if (new_bound > best_tour_cost) {
          continue;
        }
        list_t *new_tour = list_append(elem->tour, v);
        long new_contains = set_bit(elem->contains, v);
        long new_rtz = elem->rtz;
        distance_t new_cost = elem->cost + matrix_get(distances, elem->node, v);
        queue_elem_t *new_elem = queue_elem_create(new_tour, new_cost, new_bound, elem->length + 1, v, new_rtz, new_contains);

        new_elem->rtz = set_bit(new_elem->rtz, new_elem->node);
        if (new_elem->rtz == 0 && new_elem->length != N) {
          queue_elem_free(new_elem);
          continue;
        }
        queue_push(queue, (void *) new_elem);
      }
    }
    queue_elem_free(elem);
  }

  priority_queue_t *new_queue = discard(queue);
  queue_delete(queue);
  queue = new_queue;

  while (queue->size > 0) {
    queue_elem_t *elem = (queue_elem_t *) queue_pop(queue);
    //queue_elem_print(elem);
    //printf("\n");
    if (elem->bound >= best_tour_cost) {
      int * best_tour_ret = (int *) malloc(sizeof(int) * (N + 1));
      memcpy(best_tour_ret, best_tour->elements, sizeof(int) * best_tour->count);
      tsp_ret_t ret = { best_tour_ret, best_tour_cost, best_tour->count };
      queue_elem_free(elem);
      list_free(best_tour);
      return ret;
    }
    if (elem->length == N && matrix_get(distances, elem->node, 0) != -1) {
      if (elem->cost + matrix_get(distances, elem->node, 0) < best_tour_cost) {
        list_free(best_tour);
        best_tour = list_append(elem->tour, 0);
        best_tour_cost = elem->cost + matrix_get(distances, elem->node, 0);
      }
    } else {
      for (int v = 0; v < N;  v++) {
        int cost = matrix_get(distances, elem->node, v);
        if (get_bit(elem->contains, v) || cost == -1) {
          continue;
        }
        distance_t new_bound = calculate_new_bound(distances, elem->bound, elem->node, v);
        if (new_bound > best_tour_cost) {
          continue;
        }
        list_t *new_tour = list_append(elem->tour, v);
        long new_contains = set_bit(elem->contains, v);
        long new_rtz = elem->rtz;
        distance_t new_cost = elem->cost + matrix_get(distances, elem->node, v);
        queue_elem_t *new_elem = queue_elem_create(new_tour, new_cost, new_bound, elem->length + 1, v, new_rtz, new_contains);

        new_elem->rtz = set_bit(new_elem->rtz, new_elem->node);
        if (new_elem->rtz == 0 && new_elem->length != N) {
          queue_elem_free(new_elem);
          continue;
        }
        queue_push(queue, (void *) new_elem);
      }
    }
    queue_elem_free(elem);
  }
  int *best_tour_ret = (int *) malloc(sizeof(int) * (N + 1));
  memcpy(best_tour_ret, best_tour->elements, sizeof(int) * best_tour->count);
  tsp_ret_t ret = { best_tour_ret, best_tour_cost, best_tour->count };
  list_free(best_tour);
  return ret;
}

int main(int argc, char **argv) {
  if (argc < 3) {
    fprintf(stderr, "too few arguments\n");
    fprintf(stderr, "USAGE: ./tsp <matrix.in> best_cost\n");
    exit(EXIT_FAILURE);
  }
  distance_t best_tour_cost = atof(argv[2]);
  worst_cost = best_tour_cost;
  FILE *fp = fopen(argv[1], "r");
  matrix_t *distances = make_matrix_from_file(fp);
  fclose(fp);
  int n = matrix_num_columns(distances);

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &pid);
  MPI_Barrier(MPI_COMM_WORLD);
  double exec_time = -MPI_Wtime();

  tsp_ret_t best_ret;
  if (pid == 0) {
    tsp_ret_t *all_rets = malloc(sizeof(tsp_ret_t) * num_procs);
    tsp_ret_t ret = tspbb(distances, n, best_tour_cost);
    printf("%d finished\n", pid);
    all_rets[0] = ret;
    for (int i = 1; i < num_procs; i++) {
      int *best_tour = malloc(sizeof(int) * (n + 1));
      MPI_Recv(best_tour, n + 1, MPI_INT, i, RECV_TOUR_TAG, MPI_COMM_WORLD, NULL);
      MPI_Recv(&all_rets[i], sizeof(tsp_ret_t), MPI_BYTE, i, RECV_RET_TAG, MPI_COMM_WORLD, NULL);
      all_rets[i].best_tour = best_tour;
    }

    best_ret = ret;
    for (int i = 0; i < num_procs; i++) {
      ret = all_rets[i];
      if (ret.best_tour_cost < best_ret.best_tour_cost) {
        best_ret = ret;
        printf("best from %d\n", i);
      }
    }
  } else {
    tsp_ret_t ret = tspbb(distances, n, best_tour_cost);
    printf("%d finished\n", pid);
    MPI_Send(ret.best_tour, ret.count, MPI_INT, 0, RECV_TOUR_TAG, MPI_COMM_WORLD);
    MPI_Send(&ret, sizeof(tsp_ret_t), MPI_BYTE, 0, RECV_RET_TAG, MPI_COMM_WORLD);
  }
  exec_time += MPI_Wtime();
  MPI_Finalize();

  if (pid != 0) exit(EXIT_SUCCESS);

  fprintf(stderr, "%.lfs\n", exec_time);

  if (best_ret.best_tour == NULL || best_ret.count != n + 1) {
    printf("NO SOLUTION\n");
  } else {
    printf("%.1lf\n", best_ret.best_tour_cost);
    for (int i = 0; i < best_ret.count; i++) {
      printf("%d ", best_ret.best_tour[i]);
    }
    printf("\n");
  }
  free(best_ret.best_tour);
  matrix_free(distances);
  matrix_free(mins);
}