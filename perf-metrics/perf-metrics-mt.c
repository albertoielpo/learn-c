/**
 * Performance metrics multithread
 * Simple multithread program that calculates the CPU normalized score
 *
 * This benchmark tests:
 * - Integer arithmetic performance
 * - Floating-point operations (sqrt, sin)
 * - Memory bandwidth (sequential and random access)
 * - Multi-core scaling and resource contention
 *
 * Each thread runs the FULL workload to stress all cores simultaneously,
 * revealing real-world bottlenecks like memory bandwidth saturation,
 * cache contention, and thermal throttling.
 *
 * Usage:
 *   ./perf-metrics-mt           # Use all available CPUs
 *   ./perf-metrics-mt 4         # Use only 4 CPUs
 *
 * Compile with:
 * gcc -Wall -Wextra -Wpedantic -g -std=c99 perf-metrics-mt.c -lm -lpthread
 *
 * @author Alberto Ielpo <alberto.ielpo@gmail.com>
 */
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <sys/utsname.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

#define PERF_METRICS_VERSION "1.1"

// Benchmark configuration
#define ITERATIONS 100000000 // Number of operations per benchmark
#define ARRAY_SIZE 10000000  // Array size for memory tests (10M elements)

/**
 * Thread result structure
 * Holds timing and score data for each thread's benchmark run
 */
typedef struct
{
    int thread_id;     // Thread identifier (0 to num_threads-1)
    int total_threads; // Total number of threads running
    double int_time;   // Time for integer operations (seconds)
    double float_time; // Time for floating-point operations (seconds)
    double mem_time;   // Time for memory operations (seconds)
    double total_time; // Total benchmark time (seconds)
    double score;      // Normalized performance score (10000 / total_time)
} thread_result_t;

/**
 * Integer arithmetic benchmark
 * Tests CPU performance on basic integer operations (add, subtract, XOR)
 *
 * The volatile keyword prevents the compiler from optimizing away the loop
 * since the result is never used. This ensures we actually measure the
 * CPU performance rather than compiler optimization.
 *
 * @param thread_id Unique thread identifier (adds variation to prevent optimization)
 * @param iterations Number of loop iterations to perform
 * @return Elapsed time in seconds
 */
double benchmark_int_ops(int thread_id, int64_t iterations)
{
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // volatile prevents compiler from optimizing away the loop
    volatile int64_t result = 0;
    for (int64_t ii = 0; ii < iterations; ii++)
    {
        result += ii * 13 + thread_id; // Addition and multiplication
        result -= ii / 7;              // Subtraction and division
        result ^= ii;                  // Bitwise XOR
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    // Calculate elapsed time: seconds + nanoseconds converted to seconds
    // 1e9 = 1,000,000,000 nanoseconds per second
    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1e9;
    return elapsed;
}

/**
 * Floating-point benchmark
 * Tests CPU performance on math-intensive floating-point operations
 *
 * This stresses the FPU (Floating Point Unit) with expensive operations
 * like sqrt() and sin() which are computationally intensive.
 *
 * @param thread_id Unique thread identifier
 * @param iterations Number of loop iterations to perform
 * @return Elapsed time in seconds
 */
double benchmark_float_ops(int thread_id, int64_t iterations)
{
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // volatile prevents compiler from optimizing away the calculations
    volatile double result = (double)thread_id;
    for (int64_t ii = 1; ii < iterations; ii++)
    {
        result += sqrt((double)ii);       // Square root (expensive operation)
        result *= 1.0001;                 // Multiplication
        result = sin(result / 1000000.0); // Sine function (very expensive)
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1e9;
    return elapsed;
}

/**
 * Memory operations benchmark
 * Tests memory subsystem performance (bandwidth, latency, cache behavior)
 *
 * Performs three types of memory access patterns:
 * 1. Sequential write - tests write bandwidth
 * 2. Sequential read - tests read bandwidth and cache efficiency
 * 3. Random access - tests cache misses and memory latency
 *
 * When multiple threads run this simultaneously, it reveals memory
 * bandwidth saturation - a common bottleneck in multi-core systems.
 *
 * @param thread_id Unique thread identifier
 * @param array_size Number of elements in test array
 * @return Elapsed time in seconds, or -1.0 on allocation failure
 */
double benchmark_memory_ops(int thread_id, int64_t array_size)
{
    struct timespec start, end;

    // Calculate memory needed (8 bytes per int64_t element)
    size_t bytes_needed = (size_t)array_size * sizeof(int64_t);

    // Allocate memory from heap (not stack - stack is limited to ~8MB)
    int64_t *array = (int64_t *)malloc(bytes_needed);

    if (!array)
    {
        fprintf(stderr, "Thread %d: Memory allocation failed\n", thread_id);
        return -1.0;
    }

    // Touch all memory to force actual allocation
    // Linux uses lazy allocation - memory isn't really allocated until first use
    memset(array, 0, bytes_needed);

    clock_gettime(CLOCK_MONOTONIC, &start);

    // Test 1: Sequential write (tests write bandwidth)
    for (int64_t ii = 0; ii < array_size; ii++)
        array[ii] = ii * 2 + thread_id;

    // Test 2: Sequential read and accumulate (tests read bandwidth and cache)
    // volatile prevents optimization of the accumulation
    volatile int64_t sum = 0;
    for (int64_t ii = 0; ii < array_size; ii++)
        sum += array[ii];

    // Test 3: Random access pattern (tests cache misses and latency)
    // Uses prime number (7919) to create pseudo-random access pattern
    // This pattern causes many cache misses, stressing memory latency
    for (int64_t ii = 0; ii < array_size / 10; ii++)
    {
        // Use long long (LL) suffix to prevent integer overflow
        int64_t idx = (ii * 7919LL + thread_id) % array_size;
        array[idx] = array[idx] * 2 + 1;
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    // Always free allocated memory to prevent memory leaks
    free(array);

    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1e9;
    return elapsed;
}

/**
 * Thread worker function
 * Each thread runs the complete benchmark suite independently
 *
 * All threads run simultaneously with the FULL workload to stress
 * the entire system and reveal bottlenecks:
 * - Memory bandwidth saturation
 * - Cache contention (L3 cache is shared)
 * - Thermal throttling
 * - Power delivery limits
 *
 * @param arg Pointer to thread_result_t structure
 * @return NULL (required by pthread API, return value not used)
 */
void *thread_benchmark(void *arg)
{
    thread_result_t *result = (thread_result_t *)arg;
    int tid = result->thread_id;

    // Each thread does FULL workload to stress all cores simultaneously
    // This reveals real-world bottlenecks when all cores are active
    int64_t iter_per_thread = ITERATIONS;
    int64_t array_per_thread = ARRAY_SIZE;

    printf("  Thread %d: starting benchmarks...\n", tid);

    // Run all three benchmarks sequentially within this thread
    result->int_time = benchmark_int_ops(tid, iter_per_thread);
    result->float_time = benchmark_float_ops(tid, iter_per_thread);
    result->mem_time = benchmark_memory_ops(tid, array_per_thread);

    // Calculate thread-specific metrics
    result->total_time = result->int_time + result->float_time + result->mem_time;
    result->score = 10000.0 / result->total_time; // Higher score = better performance

    printf("  Thread %d: completed (%.4fs, score: %.2f)\n",
           tid, result->total_time, result->score);

    return NULL;
}

/**
 * Print help message
 * Displays usage information and program description
 */
void print_help(void)
{
    printf("DESCRIPTION:\n");
    printf("  This benchmark tests CPU and memory performance using multiple threads.\n");
    printf("  Each thread runs the complete workload to stress all cores simultaneously,\n");
    printf("  revealing real-world bottlenecks like memory bandwidth saturation,\n");
    printf("  cache contention, and thermal throttling.\n\n");

    printf("USAGE:\n");
    printf("  perf-metrics-mt [OPTIONS] [NUM_CPUS]\n\n");

    printf("OPTIONS:\n");
    printf("  -h, --help    Display this help message and exit\n\n");

    printf("ARGUMENTS:\n");
    printf("  NUM_CPUS      Number of CPU cores to use (default: all available)\n");
    printf("                Must be between 1 and the number of available CPUs\n\n");

    printf("EXAMPLES:\n");
    printf("  perf-metrics-mt           # Use all available CPUs\n");
    printf("  perf-metrics-mt 4         # Use only 4 CPUs\n");
    printf("  perf-metrics-mt --help    # Display this help message\n\n");

    printf("BENCHMARK TESTS:\n");
    printf("  - Integer arithmetic (addition, multiplication, division, XOR)\n");
    printf("  - Floating-point operations (sqrt, sin)\n");
    printf("  - Memory bandwidth (sequential and random access)\n\n");

    printf("OUTPUT:\n");
    printf("  The program runs 5 iterations and reports:\n");
    printf("  - Per-thread timing and scores\n");
    printf("  - Wall time (actual elapsed time)\n");
    printf("  - Parallel efficiency (scaling effectiveness)\n");
    printf("  - Average total score (higher is better)\n\n");
}

/**
 * Print system information
 * Displays OS details and CPU count using uname system call
 *
 * @param num_cpus Number of online CPUs detected
 */
void print_system_info(int num_cpus)
{
    struct utsname sys_info;
    if (uname(&sys_info) == 0)
    {
        printf("System Information:\n");
        printf("  OS: %s\n", sys_info.sysname);      // e.g., "Linux"
        printf("  Node: %s\n", sys_info.nodename);   // Hostname
        printf("  Release: %s\n", sys_info.release); // Kernel version
        printf("  Machine: %s\n", sys_info.machine); // Architecture (x86_64, etc.)
        printf("  CPUs: %d\n", num_cpus);
        printf("\n");
    }
}

/**
 * Main benchmark orchestration
 *
 * Features:
 * - Detects CPU count automatically using sysconf()
 * - Optional command-line argument to limit CPU usage
 * - Creates one thread per selected CPU core
 * - Runs 5 iterations for statistical accuracy
 * - Reports detailed per-thread and aggregate metrics
 *
 * Command-line usage:
 *   ./perf-metrics-mt        # Use all available CPUs
 *   ./perf-metrics-mt 4      # Use only 4 CPUs (useful for testing scaling)
 *
 * Compile with:
 * gcc -Wall -Wextra -Wpedantic -g -std=c99 -o perf-metrics-mt perf-metrics-mt.c -lm -lpthread
 *
 * Static version
 * gcc -Wall -Wextra -Wpedantic -g -std=c99 -o perf-metrics-mt perf-metrics-mt.c -lm -lpthread -static
 *
 * @param argc Argument count
 * @param argv Argument vector (argv[1] = optional CPU count)
 * @return 0 on success, 1 on error
 */
int main(int argc, char const *argv[])
{
    printf("=== Performance metrics multithread v%s ===\n\n", PERF_METRICS_VERSION);
    // Check for help flag before any other processing
    if (argc > 1 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0))
    {
        print_help();
        return 0;
    }

    // Auto-detect number of online CPUs using POSIX sysconf
    // _SC_NPROCESSORS_ONLN returns the number of processors currently online
    int64_t num_cpus = sysconf(_SC_NPROCESSORS_ONLN);
    if (num_cpus <= 0)
    {
        fprintf(stderr, "Failed to detect CPU count, defaulting to 1\n");
        num_cpus = 1;
    }

    // Store the machine's total CPU count for final reporting
    int64_t machine_cpus = num_cpus;
    print_system_info((int)num_cpus);

    // Parse command-line argument to optionally limit CPU usage
    // This is useful for testing scaling behavior or limiting resource usage
    if (argc > 1)
    {
        // Convert string argument to integer
        int selected_cpu = atoi(argv[1]);

        // Validate the CPU count
        if (selected_cpu < 1 || selected_cpu > num_cpus)
        {
            fprintf(stderr, "Invalid selected cpu: must be between 1 and %ld\n\n", num_cpus);
            print_help();
            return 1;
        }

        // Override detected CPU count with user-specified value
        num_cpus = selected_cpu;
        printf("User selected: %ld CPUs (out of %ld available)\n\n", num_cpus, machine_cpus);
    }

    // Display benchmark configuration
    printf("Configuration:\n");
    printf("  Iterations per thread: %ld (full workload)\n", (int64_t)ITERATIONS);
    printf("  Array size per thread: %ld elements (%.2f MB)\n",
           (int64_t)ARRAY_SIZE,
           (ARRAY_SIZE * sizeof(int64_t)) / (1024.0 * 1024.0));
    printf("  Total memory used: %.2f MB\n",
           (ARRAY_SIZE * sizeof(int64_t) * num_cpus) / (1024.0 * 1024.0));
    printf("\n");

    // Allocate thread resources dynamically based on CPU count
    pthread_t *threads = malloc(num_cpus * sizeof(pthread_t));
    thread_result_t *results = malloc(num_cpus * sizeof(thread_result_t));

    if (!threads || !results)
    {
        fprintf(stderr, "Failed to allocate thread resources\n");
        return 1;
    }

    // Accumulators for averaging results across 5 runs
    double total_acc_score = 0.0;
    double total_acc_time = 0.0;

    // Run benchmark 5 times for statistical stability
    // Multiple runs help average out system noise and give more reliable results
    for (int test = 0; test < 5; test++)
    {
        printf("=== Test %d ===\n", test + 1);

        // Measure wall clock time for the entire parallel execution
        // Wall time = actual elapsed time (different from CPU time)
        struct timespec test_start, test_end;
        clock_gettime(CLOCK_MONOTONIC, &test_start);

        // Create and launch all threads
        for (int64_t i = 0; i < num_cpus; i++)
        {
            // Initialize thread parameters
            results[i].thread_id = i;
            results[i].total_threads = num_cpus;

            // pthread_create: creates a new thread that executes thread_benchmark
            // Parameters: thread handle, attributes (NULL=default), function, argument
            if (pthread_create(&threads[i], NULL, thread_benchmark, &results[i]) != 0)
            {
                fprintf(stderr, "Failed to create thread %ld\n", i);
                return 1;
            }
        }

        // Wait for all threads to complete (synchronization barrier)
        // pthread_join blocks until the specified thread terminates
        for (int64_t i = 0; i < num_cpus; i++)
            pthread_join(threads[i], NULL);

        // Record end time
        clock_gettime(CLOCK_MONOTONIC, &test_end);

        // Calculate wall time (actual elapsed time)
        double wall_time = (test_end.tv_sec - test_start.tv_sec) +
                           (test_end.tv_nsec - test_start.tv_nsec) / 1e9;

        // Aggregate results from all threads
        double total_int_time = 0.0, total_float_time = 0.0, total_mem_time = 0.0;
        double total_time = 0.0, total_score = 0.0;

        // Display individual thread results
        printf("\nPer-Thread Results:\n");
        for (int64_t i = 0; i < num_cpus; i++)
        {
            printf("  Thread %ld: Int=%.4fs Float=%.4fs Mem=%.4fs Total=%.4fs Score=%.2f\n",
                   i, results[i].int_time, results[i].float_time,
                   results[i].mem_time, results[i].total_time, results[i].score);

            // Sum up all thread times (this is CPU time, not wall time)
            total_int_time += results[i].int_time;
            total_float_time += results[i].float_time;
            total_mem_time += results[i].mem_time;
            total_time += results[i].total_time;
            total_score += results[i].score;
        }

        // Calculate aggregate metrics
        double avg_score = total_score / num_cpus;

        // Parallel efficiency: measures how well threads scale
        // Formula: (Average thread time) / (Wall time) * 100
        // 100% = perfect scaling (wall time = single thread time)
        // <100% = resource contention (memory bandwidth, cache, thermal throttling)
        double parallel_efficiency = (total_time / num_cpus) / wall_time * 100.0;

        printf("\nAggregate Results:\n");
        printf("  Wall Time: %.4f seconds\n", wall_time);
        printf("  CPU Time (sum): %.4f seconds\n", total_time);
        printf("  Avg Thread Score: %.2f\n", avg_score);
        printf("  Total Score: %.2f\n", total_score);
        printf("  Parallel Efficiency: %.1f%%\n", parallel_efficiency);
        printf("\n");

        // Accumulate for averaging across all 5 runs
        total_acc_time += wall_time;
        total_acc_score += total_score;
    }

    // Display final averaged results across all 5 runs
    printf("=== Final Results (5 runs average) ===\n");
    printf("CPU used %ld/%ld\n", num_cpus, machine_cpus);
    printf("Avg Wall Time: %.4f seconds\n", total_acc_time / 5.0);
    printf("Avg Total Score: %.2f (higher is better)\n", total_acc_score / 5.0);
    printf("Score per Core: %.2f\n", (total_acc_score / 5.0) / num_cpus);

    // Cleanup: free dynamically allocated memory to prevent leaks
    free(threads);
    free(results);

    return 0;
}