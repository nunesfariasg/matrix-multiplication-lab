
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <limits.h>

#include "matrix.h"
#include "sequential.h"
#include "parallel.h"

#define WARMUP_RUNS 5
#define BENCHMARK_RUNS 30
#define TOLERANCE 1e-8

typedef struct {
    const char *name;
    int threads;
} Configuration;

static double get_time(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) +
           (end.tv_nsec - start.tv_nsec) / 1e9;
}

static Matrix multiply(
    const Matrix *A,
    const Matrix *B,
    int threads
) {
    return threads == 0
        ? matrix_multiply_sequential(A, B)
        : matrix_multiply_parallel(A, B, threads);
}

static int verify_results(const Matrix *reference, const Matrix *result) {
    if (reference == NULL || result == NULL ||
        reference->data == NULL || result->data == NULL ||
        reference->rows != result->rows ||
        reference->cols != result->cols) {
        return 0;
    }

    size_t total = (size_t) reference->rows * reference->cols;

    for (size_t i = 0; i < total; i++) {
        double a = reference->data[i];
        double b = result->data[i];

        if (!isfinite(a) || !isfinite(b)) {
            return 0;
        }

        double tolerance = TOLERANCE * fmax(1.0, fabs(a));

        if (fabs(a - b) > tolerance) {
            return 0;
        }
    }

    return 1;
}

static const char *get_filename(const char *path) {
    const char *filename = strrchr(path, '/');
    return filename ? filename + 1 : path;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file.txt>\n", argv[0]);
        return EXIT_FAILURE;
    }

    Matrix A = {0};
    Matrix B = {0};
    Matrix C_seq_ref = {0};

    if (matrix_read_file(argv[1], &A, &B) != 0) {
        return EXIT_FAILURE;
    }

    long online_cpus = sysconf(_SC_NPROCESSORS_ONLN);

    if (online_cpus < 1) {
        fprintf(stderr, "Error: Could not determine CPU count.\n");
        matrix_free(&A);
        matrix_free(&B);
        return EXIT_FAILURE;
    }

    if (online_cpus > INT_MAX) {
        online_cpus = INT_MAX;
    }

    const Configuration configs[] = {
        {"sequential", 0},
        {"2_threads", 2},
        {"4_threads", 4},
        {"8_threads", 8},
        {"max_cpus", (int) online_cpus}
    };

    const int num_configs = sizeof(configs) / sizeof(configs[0]);
    const char *filename = get_filename(argv[1]);

    int valid = 1;

    /* Warm-up and reference generation. */
    for (int c = 0; c < num_configs && valid; c++) {
        for (int w = 0; w < WARMUP_RUNS; w++) {
            Matrix C = multiply(&A, &B, configs[c].threads);

            if (C.data == NULL) {
                fprintf(stderr, "Error: Multiplication failed (%s).\n",
                        configs[c].name);
                valid = 0;
                break;
            }

            if (c == 0 && w == 0) {
                C_seq_ref = C;
                continue;
            }

            if (!verify_results(&C_seq_ref, &C)) {
                fprintf(stderr,
                        "Error: Invalid result (%s, warm-up %d).\n",
                        configs[c].name, w + 1);

                valid = 0;
            }

            matrix_free(&C);

            if (!valid) {
                break;
            }
        }
    }

    /* Benchmark. */
    for (int c = 0; c < num_configs && valid; c++) {
        const Configuration config = configs[c];

        for (int r = 1; r <= BENCHMARK_RUNS; r++) {
            struct timespec start, end;

            Matrix C = {0};

            if (clock_gettime(CLOCK_MONOTONIC, &start) != 0) {
                perror("Error reading start time");
                valid = 0;
                break;
            }

            C = multiply(&A, &B, config.threads);

            if (clock_gettime(CLOCK_MONOTONIC, &end) != 0) {
                perror("Error reading end time");
                matrix_free(&C);
                valid = 0;
                break;
            }

            double elapsed = get_time(start, end);

            /* Validate outside the timed region. */
            if (C.data == NULL || !verify_results(&C_seq_ref, &C)) {
                fprintf(stderr,
                        "Error: Invalid benchmark result (%s, run %d).\n",
                        config.name, r);

                matrix_free(&C);
                valid = 0;
                break;
            }

            printf("%s,%s,%d,%d,%.12f\n",
                   filename,
                   config.name,
                   config.threads,
                   r,
                   elapsed);

            matrix_free(&C);
        }
    }

    matrix_free(&A);
    matrix_free(&B);
    matrix_free(&C_seq_ref);

    return valid ? EXIT_SUCCESS : EXIT_FAILURE;
}
