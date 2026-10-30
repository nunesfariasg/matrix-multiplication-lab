#include <stdio.h>
#include "sequential.h"

Matrix matrix_multiply_sequential(const Matrix *A, const Matrix *B) {
    Matrix C = {0};

    if (A == NULL || B == NULL ||
        A->data == NULL || B->data == NULL ||
        A->cols != B->rows) {
        fprintf(stderr, "Error: Invalid matrices for multiplication.\n");
        return C;
        }

    C = matrix_create(A->rows, B->cols);

    if (C.data == NULL) {
        fprintf(stderr, "Error: Failed to allocate result matrix.\n");
        return C;
    }

    for (int i = 0; i < A->rows; i++) {
        for (int j = 0; j < B->cols; j++) {
            double sum = 0.0;

            for (int k = 0; k < A->cols; k++) {
                double a = A->data[(size_t)i * A->cols + k];
                double b = B->data[(size_t)k * B->cols + j];

                sum += a * b;
            }

            C.data[(size_t)i * C.cols + j] = sum;
        }
    }

    return C;
}
