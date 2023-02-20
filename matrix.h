#ifndef _TSP_MATRIX_H
#define _TSP_MATRIX_H

#include <stdlib.h>
#include <stdio.h>


typedef struct matrix matrix_t;

matrix_t *matrix_create(int rows, int columns, int default_val);
void matrix_free(matrix_t *matrix);

int matrix_get(matrix_t *matrix, int row, int column);
void matrix_set(matrix_t *matrix, int row, int column, int value);
int matrix_num_rows(matrix_t *matrix);
int matrix_num_columns(matrix_t *matrix);

void matrix_print(matrix_t *matrix);

#endif