#ifndef MATRIX_H
#define MATRIX_H

typedef struct {
    int rows;
    int cols;
    double *data;
} Matrix;

Matrix matrix_create(int rows, int cols);

void matrix_free(Matrix *matrix);

double matrix_get(const Matrix *matrix, int row, int col);

void matrix_set(Matrix *matrix, int row, int col, double value);

void matrix_print(const Matrix *matrix);

int matrix_read_file(const char *filename, Matrix *A, Matrix *B);

#endif