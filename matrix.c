#include "matrix.h"

matrix_t *matrix_create(int rows, int columns, distance_t default_val) {
  matrix_t *matrix = (matrix_t *) malloc(sizeof(matrix_t));
  matrix->num_columns = columns;
  matrix->num_rows = rows;

  // allocate row in the matrix
  matrix->matrix_elements = (distance_t **) calloc(rows, sizeof(distance_t *));
  for (int i = 0; i < rows; i++) {
    matrix->matrix_elements[i] = (distance_t *) calloc(columns, sizeof(distance_t));
  }

  // initialize matrix values
  for (int i = 0; i < matrix->num_rows; i++) {
    for (int j = 0; j < matrix->num_columns; j++) {
      matrix->matrix_elements[i][j] = default_val;
    }
  }
  return matrix;
}

void matrix_free(matrix_t *matrix) {
  for (int i = 0; i < matrix->num_rows; i++) {
    free(matrix->matrix_elements[i]);
  }
  free(matrix->matrix_elements);
  free(matrix);
}

int matrix_num_rows(matrix_t *matrix) {
  return matrix->num_rows;
}

int matrix_num_columns(matrix_t *matrix) {
  return matrix->num_columns;
}

void matrix_print(matrix_t *matrix) {
  for (int i = 0; i < matrix_num_rows(matrix); i++) {
    for (int j = 0; j < matrix_num_columns(matrix); j++) {
      printf("%d %d %f\n", i, j, matrix_get(matrix, i, j));
    }
  }
}
