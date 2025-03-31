#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

// Number of runs to average for each test case
#define NUM_RUNS 3

// Function to get current time in microseconds
double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

/**
 * Function to allocate and initialize matrices
 */
void init_matrices(int m, int n, int k, double **A, double **B, double **C) {
    // Allocate memory for matrices
    *A = (double *)malloc(m * k * sizeof(double));
    *B = (double *)malloc(k * n * sizeof(double));
    *C = (double *)malloc(m * n * sizeof(double));
    
    if (*A == NULL || *B == NULL || *C == NULL) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    
    // Initialize A and B with random values
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < k; j++) {
            (*A)[i*k + j] = (double)rand() / RAND_MAX;
        }
    }
    
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < n; j++) {
            (*B)[i*n + j] = (double)rand() / RAND_MAX;
        }
    }
    
    // Initialize C with zeros
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            (*C)[i*n + j] = 0.0;
        }
    }
}

// Function to reset matrix C to zeros
void reset_matrix_c(double *C, int m, int n) {
    for (int i = 0; i < m * n; i++) {
        C[i] = 0.0;
    }
}

// Function to free allocated memory
void free_matrices(double *A, double *B, double *C) {
    free(A);
    free(B);
    free(C);
}

/**
 * MNK implementation (row-by-row)
 */
void mnk_gemm(int m, int n, int k, double *A, double *B, double *C) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            for (int p = 0; p < k; p++) {
                C[i*n + j] += A[i*k + p] * B[p*n + j];
            }
        }
    }
}

/**
 * MKN implementation (row-inner-column)
 */
void mkn_gemm(int m, int n, int k, double *A, double *B, double *C) {
    for (int i = 0; i < m; i++) {
        for (int p = 0; p < k; p++) {
            double a_ip = A[i*k + p];
            for (int j = 0; j < n; j++) {
                C[i*n + j] += a_ip * B[p*n + j];
            }
        }
    }
}

/**
 * NMK implementation (column-by-column)
 */
void nmk_gemm(int m, int n, int k, double *A, double *B, double *C) {
    for (int j = 0; j < n; j++) {
        for (int i = 0; i < m; i++) {
            for (int p = 0; p < k; p++) {
                C[i*n + j] += A[i*k + p] * B[p*n + j];
            }
        }
    }
}

/**
 * NKM implementation (column-inner-row)
 */
void nkm_gemm(int m, int n, int k, double *A, double *B, double *C) {
    for (int j = 0; j < n; j++) {
        for (int p = 0; p < k; p++) {
            double b_pj = B[p*n + j];
            for (int i = 0; i < m; i++) {
                C[i*n + j] += A[i*k + p] * b_pj;
            }
        }
    }
}

/**
 * KMN implementation (inner-row-column)
 */
void kmn_gemm(int m, int n, int k, double *A, double *B, double *C) {
    for (int p = 0; p < k; p++) {
        for (int i = 0; i < m; i++) {
            double a_ip = A[i*k + p];
            for (int j = 0; j < n; j++) {
                C[i*n + j] += a_ip * B[p*n + j];
            }
        }
    }
}

/**
 * KNM implementation (inner-column-row)
 */
void knm_gemm(int m, int n, int k, double *A, double *B, double *C) {
    for (int p = 0; p < k; p++) {
        for (int j = 0; j < n; j++) {
            double b_pj = B[p*n + j];
            for (int i = 0; i < m; i++) {
                C[i*n + j] += A[i*k + p] * b_pj;
            }
        }
    }
}

int main() {
    // Seed the random number generator
    srand(time(NULL));
    
    // Matrix sizes to test
    int sizes[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 200, 300, 400};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
    // All loop orderings
    typedef void (*gemm_func)(int, int, int, double*, double*, double*);
    gemm_func funcs[] = {mnk_gemm, mkn_gemm, nmk_gemm, nkm_gemm, kmn_gemm, knm_gemm};
    const char *func_names[] = {"MNK", "MKN", "NMK", "NKM", "KMN", "KNM"};
    int num_funcs = sizeof(funcs) / sizeof(funcs[0]);
    
    // Create results CSV file
    FILE *results_file = fopen("gemm_times.csv", "w");
    if (results_file == NULL) {
        fprintf(stderr, "Error opening results file\n");
        return 1;
    }
    
    // Write CSV headers
    fprintf(results_file, "Matrix Size");
    for (int i = 0; i < num_funcs; i++) {
        fprintf(results_file, ",%s", func_names[i]);
    }
    fprintf(results_file, "\n");
    
    // Run benchmarks for each matrix size
    for (int s = 0; s < num_sizes; s++) {
        int size = sizes[s];
        int m = size, n = size, k = size;
        
        printf("Testing matrices of size %d x %d...\n", size, size);
        
        fprintf(results_file, "%d", size);
        
        // Allocate and initialize matrices
        double *A, *B, *C;
        init_matrices(m, n, k, &A, &B, &C);
        
        // Benchmark each implementation
        for (int i = 0; i < num_funcs; i++) {
            double total_time = 0.0;
            
            // Run the implementation NUM_RUNS times and average the results
            for (int run = 0; run < NUM_RUNS; run++) {
                // Reset matrix C to zeros for each run
                reset_matrix_c(C, m, n);
                
                // Measure execution time
                double start_time = get_time();
                funcs[i](m, n, k, A, B, C);
                double end_time = get_time();
                
                total_time += (end_time - start_time);
            }
            
            // Calculate the average execution time
            double avg_time = total_time / NUM_RUNS;
            
            // Write results to CSV file
            fprintf(results_file, ",%.6f", avg_time);
            
            // Print results to console
            printf("  %s: %.6f s\n", func_names[i], avg_time);
        }
        
        fprintf(results_file, "\n");
        fflush(results_file);
        
        // Free allocated memory
        free_matrices(A, B, C);
    }
    
    // Close file
    fclose(results_file);
    
    printf("\nBenchmarking complete. Results saved to gemm_times.csv\n");
    
    return 0;
}