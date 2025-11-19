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

#define ITERATIONS 100000000
#define ARRAY_SIZE 10000000

typedef struct
{
    int thread_id;
    int total_threads;
    double int_time;
    double float_time;
    double mem_time;
    double total_time;
    double score;
} thread_result_t;

// Integer arithmetic benchmark
double benchmark_int_ops(int thread_id, int64_t iterations)
{
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    volatile int64_t result = 0;
    for (int64_t ii = 0; ii < iterations; ii++)
    {
        result += ii * 13 + thread_id;
        result -= ii / 7;
        result ^= ii;
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1e9;
    return elapsed;
}

// Floating point benchmark
double benchmark_float_ops(int thread_id, int64_t iterations)
{
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    volatile double result = (double)thread_id;
    for (int64_t ii = 1; ii < iterations; ii++)
    {
        result += sqrt((double)ii);
        result *= 1.0001;
        result = sin(result / 1000000.0);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1e9;
    return elapsed;
}

// Memory operations benchmark
double benchmark_memory_ops(int thread_id, int64_t array_size)
{
    struct timespec start, end;

    size_t bytes_needed = (size_t)array_size * sizeof(int64_t);
    int64_t *array = (int64_t *)malloc(bytes_needed);

    if (!array)
    {
        fprintf(stderr, "Thread %d: Memory allocation failed\n", thread_id);
        return -1.0;
    }

    memset(array, 0, bytes_needed);

    clock_gettime(CLOCK_MONOTONIC, &start);

    // Sequential write
    for (int64_t ii = 0; ii < array_size; ii++)
        array[ii] = ii * 2 + thread_id;

    // Sequential read and accumulate
    volatile int64_t sum = 0;
    for (int64_t ii = 0; ii < array_size; ii++)
        sum += array[ii];

    // Random access pattern
    for (int64_t ii = 0; ii < array_size / 10; ii++)
    {
        int64_t idx = (ii * 7919LL + thread_id) % array_size;
        array[idx] = array[idx] * 2 + 1;
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    free(array);

    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1e9;
    return elapsed;
}

// Thread worker function
void *thread_benchmark(void *arg)
{
    thread_result_t *result = (thread_result_t *)arg;
    int tid = result->thread_id;

    // Each thread does FULL workload to stress all cores
    int64_t iter_per_thread = ITERATIONS;
    int64_t array_per_thread = ARRAY_SIZE;

    printf("  Thread %d: starting benchmarks...\n", tid);

    // Run benchmarks
    result->int_time = benchmark_int_ops(tid, iter_per_thread);
    result->float_time = benchmark_float_ops(tid, iter_per_thread);
    result->mem_time = benchmark_memory_ops(tid, array_per_thread);

    result->total_time = result->int_time + result->float_time + result->mem_time;
    result->score = 10000.0 / result->total_time;

    printf("  Thread %d: completed (%.4fs, score: %.2f)\n",
           tid, result->total_time, result->score);

    return NULL;
}

void print_system_info(int num_cpus)
{
    struct utsname sys_info;
    if (uname(&sys_info) == 0)
    {
        printf("System Information:\n");
        printf("  OS: %s\n", sys_info.sysname);
        printf("  Node: %s\n", sys_info.nodename);
        printf("  Release: %s\n", sys_info.release);
        printf("  Machine: %s\n", sys_info.machine);
        printf("  CPUs: %d\n", num_cpus);
        printf("\n");
    }
}

/**
 * Main benchmark orchestration
 * - Detects CPU count automatically
 * - Creates one thread per CPU core
 * - Runs 5 iterations for statistical accuracy
 * - Reports per-thread and aggregate metrics
 *
 * Compile with:
 * gcc -Wall -Wextra -Wpedantic -g -std=c99 perf-metrics-mt.c -lm -lpthread
 */
int main(int argc, char const *argv[])
{
    // Get number of CPU cores
    int64_t num_cpus = sysconf(_SC_NPROCESSORS_ONLN);
    if (num_cpus <= 0)
    {
        fprintf(stderr, "Failed to detect CPU count, defaulting to 1\n");
        num_cpus = 1;
    }
    int64_t machine_cpus = num_cpus;

    printf("=== Multi-Threaded CPU Benchmark Tool ===\n\n");
    print_system_info((int)num_cpus);

    if (argc > 1)
    {
        int selected_cpu = atoi(argv[1]);
        if (selected_cpu < 1 || selected_cpu > num_cpus)
        {
            fprintf(stderr, "Invalid selected cpu\n");
            return 1;
        }
        num_cpus = selected_cpu;
    }

    printf("Configuration:\n");
    printf("  Iterations per thread: %ld (full workload)\n", (int64_t)ITERATIONS);
    printf("  Array size per thread: %ld elements (%.2f MB)\n",
           (int64_t)ARRAY_SIZE,
           (ARRAY_SIZE * sizeof(int64_t)) / (1024.0 * 1024.0));
    printf("  Total memory used: %.2f MB\n",
           (ARRAY_SIZE * sizeof(int64_t) * num_cpus) / (1024.0 * 1024.0));
    printf("\n");

    // Allocate thread resources
    pthread_t *threads = malloc(num_cpus * sizeof(pthread_t));
    thread_result_t *results = malloc(num_cpus * sizeof(thread_result_t));

    if (!threads || !results)
    {
        fprintf(stderr, "Failed to allocate thread resources\n");
        return 1;
    }

    // Run benchmark 5 times
    double total_acc_score = 0.0;
    double total_acc_time = 0.0;

    for (int test = 0; test < 5; test++)
    {
        printf("=== Test %d ===\n", test + 1);

        struct timespec test_start, test_end;
        clock_gettime(CLOCK_MONOTONIC, &test_start);

        // Create threads
        for (int64_t i = 0; i < num_cpus; i++)
        {
            results[i].thread_id = i;
            results[i].total_threads = num_cpus;

            if (pthread_create(&threads[i], NULL, thread_benchmark, &results[i]) != 0)
            {
                fprintf(stderr, "Failed to create thread %ld\n", i);
                return 1;
            }
        }

        // Wait for all threads to complete
        for (int64_t i = 0; i < num_cpus; i++)
            pthread_join(threads[i], NULL);

        clock_gettime(CLOCK_MONOTONIC, &test_end);
        double wall_time = (test_end.tv_sec - test_start.tv_sec) +
                           (test_end.tv_nsec - test_start.tv_nsec) / 1e9;

        // Calculate aggregate results
        double total_int_time = 0.0, total_float_time = 0.0, total_mem_time = 0.0;
        double total_time = 0.0, total_score = 0.0;

        printf("\nPer-Thread Results:\n");
        for (int64_t i = 0; i < num_cpus; i++)
        {
            printf("  Thread %ld: Int=%.4fs Float=%.4fs Mem=%.4fs Total=%.4fs Score=%.2f\n",
                   i, results[i].int_time, results[i].float_time,
                   results[i].mem_time, results[i].total_time, results[i].score);

            total_int_time += results[i].int_time;
            total_float_time += results[i].float_time;
            total_mem_time += results[i].mem_time;
            total_time += results[i].total_time;
            total_score += results[i].score;
        }

        double avg_score = total_score / num_cpus;
        double parallel_efficiency = (total_time / num_cpus) / wall_time * 100.0;

        printf("\nAggregate Results:\n");
        printf("  Wall Time: %.4f seconds\n", wall_time);
        printf("  CPU Time (sum): %.4f seconds\n", total_time);
        printf("  Avg Thread Score: %.2f\n", avg_score);
        printf("  Total Score: %.2f\n", total_score);
        printf("  Parallel Efficiency: %.1f%%\n", parallel_efficiency);
        printf("\n");

        total_acc_time += wall_time;
        total_acc_score += total_score;
    }

    printf("=== Final Results (5 runs average) ===\n");
    printf("CPU used %ld/%ld\n", num_cpus, machine_cpus);
    printf("Avg Wall Time: %.4f seconds\n", total_acc_time / 5.0);
    printf("Avg Total Score: %.2f (higher is better)\n", total_acc_score / 5.0);
    printf("Score per Core: %.2f\n", (total_acc_score / 5.0) / num_cpus);

    // Cleanup
    free(threads);
    free(results);

    return 0;
}