#include "matrix.h"

struct matrix {
  int num_rows;
  int num_columns;
  int **matrix_elements;
};

matrix_t *matrix_create(int rows, int columns, int default_val) {
  matrix_t *matrix = (matrix_t *) malloc(sizeof(matrix_t));
  matrix->num_columns = columns;
  matrix->num_rows = rows;

  // allocate row in the matrix
  matrix->matrix_elements = (int **) calloc(rows, sizeof(int *));
  for (int i = 0; i < rows; i++) {
    matrix->matrix_elements[i] = (int *) calloc(columns, sizeof(int));
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

int matrix_get(matrix_t *matrix, int row, int column) {
  return matrix->matrix_elements[row][column];
}

void matrix_set(matrix_t *matrix, int row, int column, int value) {
  matrix->matrix_elements[row][column] = value;
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
      printf("%d %d %d\n", i, j, matrix_get(matrix, i, j));
    }
  }
}
