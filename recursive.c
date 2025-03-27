#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/resource.h>

// Global variable to track maximum recursion depth
int max_recursion_depth = 0;
int current_depth = 0;

/**
 * Recursive factorial with stack depth tracking
 * @param n The number for which factorial is to be calculated
 * @return The factorial of n
 */
int factorial_recursive(int n) {
    // Track recursion depth for memory calculation
    current_depth++;
    if (current_depth > max_recursion_depth) {
        max_recursion_depth = current_depth;
    }
    
    // Base case
    if (n <= 1) {
        current_depth--;
        return 1;
    }
    
    // Recursive case - use repeated addition instead of multiplication
    int smaller_factorial = factorial_recursive(n - 1);
    
    // Multiply n with smaller_factorial using repeated addition
    int result = 0;
    for (int i = 0; i < n; i++) {
        result += smaller_factorial;
    }
    
    current_depth--;
    return result;
}

/**
 * Calculate stack frame size for a recursive call
 * @return Estimated size of a single stack frame in bytes
 */
size_t calculate_stack_frame_size() {
    // Parameters and return value
    size_t param_size = sizeof(int);
    
    // Local variables
    size_t local_vars = sizeof(int) * 2;  // smaller_factorial, result
    
    // Loop counter
    size_t loop_counter = sizeof(int);    // i
    
    // Function call overhead (return address, frame pointer)
    size_t call_overhead = sizeof(void*) * 2;
    
    return param_size + local_vars + loop_counter + call_overhead;
}

/**
 * Main function to test factorial
 */
int main() {
    int values[] = {3, 6, 7, 8};
    int num_values = sizeof(values) / sizeof(values[0]);
    
    // For measuring memory
    struct rusage usage_before, usage_after;
    
    printf("Recursive Factorial Implementation\n");
    printf("==================================\n");
    
    for (int i = 0; i < num_values; i++) {
        int n = values[i];
        
        // Reset depth trackers
        max_recursion_depth = 0;
        current_depth = 0;
        
        // Get memory usage before calculation
        getrusage(RUSAGE_SELF, &usage_before);
        
        // Measure time
        clock_t start = clock();
        int result = factorial_recursive(n);
        clock_t end = clock();
        
        // Get memory usage after calculation
        getrusage(RUSAGE_SELF, &usage_after);
        
        double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        
        // Calculate memory from system measurement
        long system_memory = (usage_after.ru_maxrss - usage_before.ru_maxrss) * 1024;
        
        // Calculate theoretical stack memory based on recursion depth
        size_t stack_frame_size = calculate_stack_frame_size();
        size_t theoretical_memory = stack_frame_size * max_recursion_depth;
        
        // Use the larger of the two for reporting
        long memory_used = (system_memory > 0) ? system_memory : theoretical_memory;
        
        printf("Factorial of %d = %d\n", n, result);
        printf("Time taken: %f seconds\n", cpu_time_used);
        printf("Memory used: %ld bytes\n", memory_used);
        printf("(Stack depth: %d, Frame size: %zu bytes)\n\n", 
               max_recursion_depth, stack_frame_size);
    }
    
    return 0;
}