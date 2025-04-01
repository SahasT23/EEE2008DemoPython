#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

// Number of runs to average for each test case
#define NUM_RUNS 3

// Default number of threads and block size (can alter the block size and thead count by adding threads)
#define DEFAULT_NUM_THREADS 4
#define DEFAULT_BLOCK_SIZE 32

// Thread argument structure
typedef struct {
    int thread_id;
    int num_threads;
    int m, n, k;
    int block_size;
    double *A;
    double *B;
    double *C;
} thread_args_t;

double get_time() { // Used the same method for time measurement.
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

/**
 * Function to allocate and initialise matrices
 */
void init_matrices(int m, int n, int k, double **A, double **B, double **C) {
    *A = (double *)malloc(m * k * sizeof(double)); // memory allocation for the matrices (A, B and C)
    *B = (double *)malloc(k * n * sizeof(double));
    *C = (double *)malloc(m * n * sizeof(double));
    
    if (*A == NULL || *B == NULL || *C == NULL) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    
    // Initialise A and B with random values between 0 and 1
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
    
    // Initialise C with zeros
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            (*C)[i*n + j] = 0.0;
        }
    }
}

void reset_matrix_c(double *C, int m, int n) {
    for (int i = 0; i < m * n; i++) {
        C[i] = 0.0;
    }
}

void free_matrices(double *A, double *B, double *C) {
    free(A);
    free(B);
    free(C);
}

/**
 * 1. Original MNK implementation (row-by-row)
 * Only using MNK for this, don't want the graph to look weird and jumbled together.
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
 * 2. Blocked/Tiled MNK implementation
 * Need to integrate this into my report later.
 */
void blocked_mnk_gemm(int m, int n, int k, double *A, double *B, double *C, int block_size) {
    // Iterate over 'blocks'
    for (int i0 = 0; i0 < m; i0 += block_size) {
        int i_bound = (i0 + block_size < m) ? i0 + block_size : m;
        
        for (int j0 = 0; j0 < n; j0 += block_size) {
            int j_bound = (j0 + block_size < n) ? j0 + block_size : n;
            
            for (int p0 = 0; p0 < k; p0 += block_size) {
                int p_bound = (p0 + block_size < k) ? p0 + block_size : k;
                
                // Compute within the block
                for (int i = i0; i < i_bound; i++) {
                    for (int j = j0; j < j_bound; j++) {
                        for (int p = p0; p < p_bound; p++) {
                            C[i*n + j] += A[i*k + p] * B[p*n + j];
                        }
                    }
                }
            }
        }
    }
}

/**
 * Thread function for multithreaded MNK implementation
 * (check later, maybe not the best opion)
 */
void* mt_mnk_thread(void *arg) {
    thread_args_t *args = (thread_args_t *)arg;
    int thread_id = args->thread_id;
    int num_threads = args->num_threads;
    int m = args->m;
    int n = args->n;
    int k = args->k;
    double *A = args->A;
    double *B = args->B;
    double *C = args->C;
    
    // Each thread processes a subset of rows
    int rows_per_thread = (m + num_threads - 1) / num_threads;
    int start_row = thread_id * rows_per_thread;
    int end_row = (start_row + rows_per_thread < m) ? start_row + rows_per_thread : m;
    
    // Perform MNK matrix multiplication on assigned rows
    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < n; j++) {
            for (int p = 0; p < k; p++) {
                C[i*n + j] += A[i*k + p] * B[p*n + j];
            }
        }
    }
    
    return NULL;
}

/**
 * 3. Multithreaded MNK implementation
 */
void mt_mnk_gemm(int m, int n, int k, double *A, double *B, double *C, int num_threads) {
    pthread_t threads[num_threads];
    thread_args_t args[num_threads];
    
    // Create threads
    for (int t = 0; t < num_threads; t++) {
        args[t].thread_id = t;
        args[t].num_threads = num_threads;
        args[t].m = m;
        args[t].n = n;
        args[t].k = k;
        args[t].A = A;
        args[t].B = B;
        args[t].C = C;
        
        pthread_create(&threads[t], NULL, mt_mnk_thread, &args[t]);
    }
    
    // Join threads
    for (int t = 0; t < num_threads; t++) {
        pthread_join(threads[t], NULL);
    }
}

/**
 * Thread function for combined multithreaded and blocked MNK implementation
 * Probably no t the best idea, might not be the most efficient.
 */
void* mt_blocked_mnk_thread(void *arg) {
    thread_args_t *args = (thread_args_t *)arg;
    int thread_id = args->thread_id;
    int num_threads = args->num_threads;
    int m = args->m;
    int n = args->n;
    int k = args->k;
    int block_size = args->block_size;
    double *A = args->A;
    double *B = args->B;
    double *C = args->C;
    
    // Each thread processes a subset of blocks in the i direction
    int i_blocks = (m + block_size - 1) / block_size;
    int blocks_per_thread = (i_blocks + num_threads - 1) / num_threads;
    int start_block = thread_id * blocks_per_thread;
    int end_block = (start_block + blocks_per_thread < i_blocks) ? start_block + blocks_per_thread : i_blocks;
    
    int start_i = start_block * block_size;
    int end_i = (end_block * block_size < m) ? end_block * block_size : m;
    
    // Iterate over blocks assigned to this thread.
    // Complexity is higher than just multithreading.
    for (int i0 = start_i; i0 < end_i; i0 += block_size) {
        int i_bound = (i0 + block_size < m) ? i0 + block_size : m;
        
        for (int j0 = 0; j0 < n; j0 += block_size) {
            int j_bound = (j0 + block_size < n) ? j0 + block_size : n;
            
            for (int p0 = 0; p0 < k; p0 += block_size) {
                int p_bound = (p0 + block_size < k) ? p0 + block_size : k;
                
                // Compute within the block
                for (int i = i0; i < i_bound; i++) {
                    for (int j = j0; j < j_bound; j++) {
                        for (int p = p0; p < p_bound; p++) {
                            C[i*n + j] += A[i*k + p] * B[p*n + j];
                        }
                    }
                }
            }
        }
    }
    
    return NULL;
}

/**
 * 4. Combined multithreaded and blocked MNK implementation
 */
void mt_blocked_mnk_gemm(int m, int n, int k, double *A, double *B, double *C, int num_threads, int block_size) {
    pthread_t threads[num_threads];
    thread_args_t args[num_threads];
    
    // Create threads
    for (int t = 0; t < num_threads; t++) {
        args[t].thread_id = t;
        args[t].num_threads = num_threads;
        args[t].m = m;
        args[t].n = n;
        args[t].k = k;
        args[t].block_size = block_size;
        args[t].A = A;
        args[t].B = B;
        args[t].C = C;
        
        pthread_create(&threads[t], NULL, mt_blocked_mnk_thread, &args[t]);
    }
    
    // Join threads
    for (int t = 0; t < num_threads; t++) {
        pthread_join(threads[t], NULL);
    }
}

int main(int argc, char *argv[]) {
    // Seed the random number generator
    srand(time(NULL));
    
    // Parse command line arguments for threads and block size
    int num_threads = DEFAULT_NUM_THREADS;
    int block_size = DEFAULT_BLOCK_SIZE;
    
    if (argc > 1) {
        num_threads = atoi(argv[1]);
        if (num_threads < 1) num_threads = 1;
    }
    
    if (argc > 2) {
        block_size = atoi(argv[2]);
        if (block_size < 1) block_size = 1;
    }
    
    printf("Running with %d threads and block size %d\n", num_threads, block_size);
    
    // Matrix sizes to test
    int sizes[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 200, 300, 400};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
    // Implementation variant names
    const char *func_names[] = {"Original MNK", "Blocked MNK", "Multithreaded MNK", "MT+Blocked MNK"};
    int num_funcs = sizeof(func_names) / sizeof(func_names[0]);
    
    // Create results CSV file
    FILE *results_file = fopen("mnk_optimized_times.csv", "w");
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
        
        // Benchmark implementations
        
        // 1. Original MNK
        double total_time = 0.0;
        for (int run = 0; run < NUM_RUNS; run++) {
            reset_matrix_c(C, m, n);
            double start_time = get_time();
            mnk_gemm(m, n, k, A, B, C);
            double end_time = get_time();
            total_time += (end_time - start_time);
        }
        double mnk_time = total_time / NUM_RUNS;
        fprintf(results_file, ",%.6f", mnk_time);
        printf("  Original MNK: %.6f s\n", mnk_time);
        
        // 2. Blocked MNK
        total_time = 0.0;
        for (int run = 0; run < NUM_RUNS; run++) {
            reset_matrix_c(C, m, n);
            double start_time = get_time();
            blocked_mnk_gemm(m, n, k, A, B, C, block_size);
            double end_time = get_time();
            total_time += (end_time - start_time);
        }
        double blocked_time = total_time / NUM_RUNS;
        fprintf(results_file, ",%.6f", blocked_time);
        printf("  Blocked MNK: %.6f s\n", blocked_time);
        
        // 3. Multithreaded MNK
        total_time = 0.0;
        for (int run = 0; run < NUM_RUNS; run++) {
            reset_matrix_c(C, m, n);
            double start_time = get_time();
            mt_mnk_gemm(m, n, k, A, B, C, num_threads);
            double end_time = get_time();
            total_time += (end_time - start_time);
        }
        double mt_time = total_time / NUM_RUNS;
        fprintf(results_file, ",%.6f", mt_time);
        printf("  Multithreaded MNK: %.6f s\n", mt_time);
        
        // 4. Combined multithreaded and blocked MNK
        total_time = 0.0;
        for (int run = 0; run < NUM_RUNS; run++) {
            reset_matrix_c(C, m, n);
            double start_time = get_time();
            mt_blocked_mnk_gemm(m, n, k, A, B, C, num_threads, block_size);
            double end_time = get_time();
            total_time += (end_time - start_time);
        }
        double mt_blocked_time = total_time / NUM_RUNS;
        fprintf(results_file, ",%.6f", mt_blocked_time);
        printf("  MT+Blocked MNK: %.6f s\n", mt_blocked_time);
        
        fprintf(results_file, "\n");
        fflush(results_file);
        
        // Free allocated memory
        free_matrices(A, B, C);
    }
    
    // Close file
    fclose(results_file);
    
    printf("\nBenchmarking complete. Results saved to mnk_optimized_times.csv\n");
    
    return 0;
}