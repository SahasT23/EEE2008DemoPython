#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 * Function to perform addition as a replacement for multiplication
 * @param a First operand
 * @param b Second operand
 * @return Result of a * b using repeated addition
 */
int add_multiply(int a, int b) {
    int result = 0;
    for (int i = 0; i < b; i++) {
        result += a;
    }
    return result;
}

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
    
    // For measuring time
    clock_t start, end;
    double cpu_time_used;
    
    printf("Iterative Factorial Implementation\n");
    printf("==================================\n");
    
    for (int i = 0; i < num_values; i++) {
        int n = values[i];
        
        // Measure time
        start = clock();
        int result = factorial_iterative(n);
        end = clock();
        
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        
        printf("Factorial of %d = %d\n", n, result);
        printf("Time taken: %f seconds\n", cpu_time_used);
        
        // Rough memory usage estimation (stack only)
        printf("Approximate memory usage: %lu bytes\n", sizeof(int) * 3); // result, i, temp
        printf("\n");
    }
    
    return 0;
}