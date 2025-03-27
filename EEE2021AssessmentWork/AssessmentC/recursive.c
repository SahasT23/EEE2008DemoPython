#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Can think of multiplication as repeated addition
int add_multiply(int a, int b) {
    int result = 0;
    for (int i = 0; i < b; i++) {
        result += a;
    }
    return result;
}

/**
 * Calculate factorial recursively without using multiplication
 * @param n The number for which factorial is to be calculated
 * @return The factorial of n
 */
int factorial_recursive(int n) {
    // Base case
    if (n <= 1) {
        return 1;
    }
    
    // Recursive case - use repeated addition instead of multiplication
    int smaller_factorial = factorial_recursive(n - 1);
    
    // Multiply n with smaller_factorial using repeated addition
    int result = 0;
    for (int i = 0; i < n; i++) {
        result += smaller_factorial;
    }
    
    return result;
}

/**
 * Function to measure the stack depth of recursive calls
 * For memory usage estimation
 */
int measure_stack_depth(int n, int current_depth) {
    if (n <= 1) {
        return current_depth;
    }
    return measure_stack_depth(n - 1, current_depth + 1);
}

/**
 * Main function to test factorial
 */
int main() {
    int values[] = {3, 6, 7, 8};
    int num_values = sizeof(values) / sizeof(values[0]);
    
    // For measuring time
    clock_t start, end;
    double cpu_time_used;
    
    printf("Recursive Factorial Implementation\n");
    printf("==================================\n");
    
    for (int i = 0; i < num_values; i++) {
        int n = values[i];
        
        // Measure time
        start = clock();
        int result = factorial_recursive(n);
        end = clock();
        
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        
        printf("Factorial of %d = %d\n", n, result);
        printf("Time taken: %f seconds\n", cpu_time_used);
        
        // Rough memory usage estimation
        int stack_depth = measure_stack_depth(n, 1);
        printf("Recursive call depth: %d\n", stack_depth);
        printf("Approximate memory usage: %lu bytes\n", 
               sizeof(int) * 3 * stack_depth); // smaller_factorial, result, i for each recursive call
        printf("\n");
    }
    
    return 0;
}