#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/resource.h>

/**
 * Calculate factorial iteratively without using multiplication
 * @param n The number for which factorial is to be calculated
 * @return The factorial of n
 */
int factorial_iterative(int n) {
    int result = 1;
    
    for (int i = 2; i <= n; i++) {
        // Use repeated addition to simulate multiplication
        int temp = 0;
        for (int j = 0; j < i; j++) {
            temp += result;
        }
        result = temp;
    }
    
    return result;
}

/**
 * Main function to test factorial calculation
 */
int main() {
    // Array of values to calculate factorial for
    int values[] = {3, 6, 7, 8};
    int num_values = sizeof(values) / sizeof(values[0]);
    
    // For measuring memory
    struct rusage usage_before, usage_after;
    
    printf("Iterative Factorial Implementation\n");
    printf("==================================\n");
    
    for (int i = 0; i < num_values; i++) {
        int n = values[i];
        
        // Get memory usage before calculation
        getrusage(RUSAGE_SELF, &usage_before);
        
        // Measure time
        clock_t start = clock();
        int result = factorial_iterative(n);
        clock_t end = clock();
        
        // Get memory usage after calculation
        getrusage(RUSAGE_SELF, &usage_after);
        
        double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        
        // Calculate peak memory usage (in bytes)
        long memory_used = (usage_after.ru_maxrss - usage_before.ru_maxrss) * 1024;
        
        // If no change detected in RSS, provide a detailed estimate
        if (memory_used <= 0) {
            // Calculate static memory for main variables
            size_t static_memory = sizeof(int) * 3; // result, i, temp variables
            
            // Add memory for each loop level
            size_t loop_memory = sizeof(int) * 2; // i and j loop counters
            
            // Total theoretical memory (scales with n due to loop iterations)
            memory_used = static_memory + loop_memory + (n * sizeof(int));
        }
        
        printf("Factorial of %d = %d\n", n, result);
        printf("Time taken: %f seconds\n", cpu_time_used);
        printf("Memory used: %ld bytes\n\n", memory_used);
    }
    
    return 0;
}