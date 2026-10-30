#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "matrix.h"

Matrix matrix_create(int rows, int cols) {
    Matrix matrix = {0};

    // Validate size.
    if (rows <= 0 || cols <= 0) {
        fprintf(stderr, "Error: Matrix dimensions must be positive.\n");
        return matrix;
    }

    // Allocate memory.
    matrix.data = calloc((size_t) rows * cols, sizeof(double));

    // validate allocation.
    if (matrix.data == NULL) {
        fprintf(stderr, "Error: Failed to allocate matrix memory.\n");
        return matrix;
    }

    matrix.rows = rows;
    matrix.cols = cols;

    return matrix;
}

void matrix_free(Matrix *matrix) {
    if (matrix == NULL) {
        return;
    }

    free(matrix->data);

    matrix->data = NULL;
    matrix->rows = 0;
    matrix->cols = 0;
}

double matrix_get(const Matrix *matrix, int row, int col) {

    // Validate matrix access.
    if (matrix == NULL || matrix->data == NULL ||
        row < 0 || row >= matrix->rows ||
        col < 0 || col >= matrix->cols) {
        fprintf(stderr, "Error: Invalid matrix access.\n");
        return 0.0;
        }

    // Maps 2D coordinates (row, column) to a linear index
    // row-major order (k = i * N + j).
    return matrix->data[(size_t) row * matrix->cols + col];
}

void matrix_set(Matrix *matrix, int row, int col, double value) {

    // Validate matrix access.
    if (matrix == NULL || matrix->data == NULL ||
        row < 0 || row >= matrix->rows ||
        col < 0 || col >= matrix->cols) {
        fprintf(stderr, "Error: Invalid matrix access.\n");
        return;
        }

    // Invert row-major order.
    matrix->data[(size_t) row * matrix->cols + col] = value;
}

void matrix_print(const Matrix *matrix) {

    // Validate matrix.
    if (matrix == NULL || matrix->data == NULL) {
        fprintf(stderr, "Error: Cannot print an invalid matrix.\n");
        return;
    }

    // Print the matrix.
    for (int i = 0; i < matrix->rows; i++) {
        for (int j = 0; j < matrix->cols; j++) {
            printf("%8.2f ", matrix_get(matrix, i, j));
        }

        printf("\n");
    }
}

int matrix_read_file(const char *filename, Matrix *A, Matrix *B) {

    // Validate arguments.
    if (filename == NULL || A == NULL || B == NULL || A == B) {
        fprintf(stderr, "Error: Invalid arguments.\n");
        return 1;
    }

    FILE *file = fopen(filename, "r");

    // Validate file
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open file '%s'.\n", filename);
        return 1;
    }

    int size;

    // Validate matrix dimension.
    if (fscanf(file, "%d", &size) != 1 || size <= 0) {
        fprintf(stderr, "Error: Invalid matrix dimension.\n");
        fclose(file);
        return 1;
    }

    // Map two matrices
    Matrix first = matrix_create(size, size);
    Matrix second = matrix_create(size, size);


    // Validate the matrix memory allocation.
    if (first.data == NULL || second.data == NULL) {
        fprintf(stderr, "Error: Failed to allocate matrix memory.\n");

        matrix_free(&first);
        matrix_free(&second);

        fclose(file);
        return 1;
    }

    size_t total = (size_t) size * size;

    // Read the first matrix.
    for (size_t i = 0; i < total; i++) {
        if (fscanf(file, "%lf", &first.data[i]) != 1) {
            fprintf(stderr, "Error: Invalid data in the first matrix.\n");

            matrix_free(&first);
            matrix_free(&second);

            fclose(file);
            return 1;
        }
    }

    // Read the second matrix.
    for (size_t i = 0; i < total; i++) {
        if (fscanf(file, "%lf", &second.data[i]) != 1) {
            fprintf(stderr, "Error: Invalid data in the second matrix.\n");

            matrix_free(&first);
            matrix_free(&second);

            fclose(file);
            return 1;
        }
    }

    // Reject unexpected content after the matrices.
    int ch;

    do {
        ch = fgetc(file);
    } while (ch != EOF && isspace((unsigned char) ch));

    if (ch != EOF || ferror(file)) {
        fprintf(stderr, "Error: Unexpected content in file.\n");

        matrix_free(&first);
        matrix_free(&second);

        fclose(file);
        return 1;
    }

    fclose(file);

    *A = first;
    *B = second;

    return 0;
}
