#ifndef PARALLEL_H
#define PARALLEL_H

#include "matrix.h"

Matrix matrix_multiply_parallel(
    const Matrix *A,
    const Matrix *B,
    int num_threads
);

#endif