#include <stdio.h>
#include <stdlib.h>
#include <time.h>  
#include <sys/time.h>  // Added these packages for timing calculations.

// Number of 'runs' to average for each test case (best of 3, average will be )
#define NUM_RUNS 3

// Function to get current time in microseconds (more accurate for measurement)
/**
 * Can use 
 */
double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

/*
Initialising the matrices for AB + C.
*/
void init_matrices(int m, int n, int k, double **A, double **B, double **C) {
    *A = (double *)malloc(m * k * sizeof(double));
    *B = (double *)malloc(k * n * sizeof(double));
    *C = (double *)malloc(m * n * sizeof(double));
    
    if (*A == NULL || *B == NULL || *C == NULL) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    
    /**
     * Initialising the values of the matrices to be between 0 and 1. 
     * Best method I have seen so far for randomisation. 
     */
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
    
    // Initialising C as a zero matrix, needed for GeMM to be done correctly.
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            (*C)[i*n + j] = 0.0;
        }
    }
}

// Resets the matrix to 0 every time.
void reset_matrix_c(double *C, int m, int n) {
    for (int i = 0; i < m * n; i++) {
        C[i] = 0.0;
    }
}

// Clears the matrices for the next calculation. 
void free_matrices(double *A, double *B, double *C) {
    free(A);
    free(B);
    free(C);
}

/**
 * MNK implementation (row-by-row) Time Complexity of O(n^3).
 * The reason why it is n^3 is becuase of the 3 nested 'for' loops,
 * The outermost loop runs 'm' times.
 * The middle loop runs 'n' times for each iteration of the outer loop.
 * The innermost loop runs 'k' times for each iteration of the middle loop, 
 * So the Big O notation will amount to  O(m * n * k), -> O(n^3).
 * Even if the loop ordering changes, it will always be O(n^3). 
 * 
 * Space complexity is much more simpler for this algorithm/loop ordering.
 * Since no additional data structures are allocated within the function 
 * (only the given matrices are used), the space complexity is O(1), regardless of loop ordering.
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
 * MKN implementation (row-inner-column) Time Complexity of O(n^3).
 * In practice, there should be very minimal difference between the different orderings,
 * but locality should be considered for each different ordering. 
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
 * NMK implementation (column-by-column) Time Complexity of O(n^3)
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
 * NKM implementation (column-inner-row) Time Complexity of O(n^3)
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
 * KMN implementation (inner-row-column) Time Complexity of O(n^3)
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
 * KNM implementation (inner-column-row) Time Complexity of O(n^3)
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
    // Random number generation.
    srand(time(NULL));
    
    /*
    Matrix sizes to test (The jump between matrix sizes is on purpose, 
    can clearly see the time increasing as it is a cubic relationship). 
    */
    int sizes[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 200, 300, 400};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
    // All loop orderings that I have used.
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
    
    // CSV file headers, easier for me to use for graph plotting purposes. Check the python scripts for plotting. 
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
        
        // Initialise matrices correctly
        double *A, *B, *C;
        init_matrices(m, n, k, &A, &B, &C);
        
        // Benchmark each loop ordering correctly
        for (int i = 0; i < num_funcs; i++) {
            double total_time = 0.0;
            
            // Run the implementation NUM_RUNS (3, but you can alter it) times and average the results
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
    
    fclose(results_file);
    
    printf("\nBenchmarking complete. Results saved to gemm_times.csv\n");
    
    return 0;
}