#ifndef _TSP_MATRIX_H
#define _TSP_MATRIX_H

#include <stdlib.h>
#include <stdio.h>

#define matrix_get(m,r,c) (m->matrix_elements[r][c])
#define matrix_set(m,r,c,v) (m->matrix_elements[r][c] = v)

typedef double distance_t;

typedef struct {
  int num_rows;
  int num_columns;
  distance_t **matrix_elements;
} matrix_t;

matrix_t *matrix_create(int rows, int columns, distance_t default_val);
void matrix_free(matrix_t *matrix);

int matrix_num_rows(matrix_t *matrix);
int matrix_num_columns(matrix_t *matrix);

void matrix_print(matrix_t *matrix);

#endif