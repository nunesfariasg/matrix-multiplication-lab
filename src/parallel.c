#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "parallel.h"

typedef struct {
    const Matrix *A;
    const Matrix *B;
    Matrix *C;

    int start_row;
    int end_row;
} ThreadData;

static void *multiply_rows(void *arg) {
    ThreadData *data = arg;

    const Matrix *A = data->A;
    const Matrix *B = data->B;
    Matrix *C = data->C;

    for (int i = data->start_row; i < data->end_row; i++) {
        for (int j = 0; j < B->cols; j++) {
            double sum = 0.0;

            for (int k = 0; k < A->cols; k++) {
                double a = A->data[(size_t)i * A->cols + k];
                double b = B->data[(size_t)k * B->cols + j];

                sum += a * b;
            }

            C->data[(size_t)i * C->cols + j] = sum;
        }
    }

    return NULL;
}

Matrix matrix_multiply_parallel(
    const Matrix *A,
    const Matrix *B,
    int num_threads
) {
    Matrix C = {0};

    if (A == NULL || B == NULL ||
        A->data == NULL || B->data == NULL ||
        A->cols != B->rows ||
        num_threads <= 0) {
        fprintf(stderr, "Error: Invalid arguments for parallel multiplication.\n");
        return C;
    }

    C = matrix_create(A->rows, B->cols);

    if (C.data == NULL) {
        fprintf(stderr, "Error: Failed to allocate result matrix.\n");
        return C;
    }

    /*
     * There is no benefit in assigning more threads
     * than there are rows to calculate.
     */
    if (num_threads > A->rows) {
        num_threads = A->rows;
    }

    ThreadData *args = calloc((size_t)num_threads, sizeof(*args));

    if (args == NULL) {
        fprintf(stderr, "Error: Failed to allocate thread data.\n");
        matrix_free(&C);
        return C;
    }

    pthread_t *threads = NULL;

    if (num_threads > 1) {
        threads = malloc((size_t)(num_threads - 1) * sizeof(*threads));

        if (threads == NULL) {
            fprintf(stderr, "Error: Failed to allocate threads.\n");
            free(args);
            matrix_free(&C);
            return C;
        }
    }

    /*
     * Divide the rows into disjoint ranges.
     */
    for (int i = 0; i < num_threads; i++) {
        args[i].A = A;
        args[i].B = B;
        args[i].C = &C;

        args[i].start_row = i * A->rows / num_threads;
        args[i].end_row = (i + 1) * A->rows / num_threads;
    }

    /*
     * Create all threads except the last one.
     * The calling thread will process the last range.
     */
    int created_threads = 0;

    for (int i = 0; i < num_threads - 1; i++) {
        int error = pthread_create(
            &threads[i],
            NULL,
            multiply_rows,
            &args[i]
        );

        if (error != 0) {
            fprintf(stderr, "Error: Failed to create thread.\n");

            for (int j = 0; j < created_threads; j++) {
                pthread_join(threads[j], NULL);
            }

            free(threads);
            free(args);
            matrix_free(&C);

            return (Matrix){0};
        }

        created_threads++;
    }

    /*
     * The main thread also performs useful work.
     */
    multiply_rows(&args[num_threads - 1]);

    /*
     * Wait for all created threads to finish.
     */
    for (int i = 0; i < created_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    free(args);

    return C;
}