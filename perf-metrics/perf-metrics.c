#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <sys/utsname.h>
#include <stdint.h>

#define ITERATIONS 100000000
#define ARRAY_SIZE 10000000

// Arithmetic benchmark
double benchmark_int_ops(void)
{
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    volatile int64_t result = 0;
    for (int64_t ii = 0; ii < ITERATIONS; ii++)
    {
        result += ii * 13;
        result -= ii / 7;
        result ^= ii;
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1e9;
    return elapsed;
}

// Floating point benchmark
double benchmark_float_ops(void)
{
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    volatile double result = 0.0;
    for (int64_t ii = 1; ii < ITERATIONS; ii++)
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
double benchmark_memory_ops(void)
{
    struct timespec start, end;

    size_t bytes_needed = (size_t)ARRAY_SIZE * sizeof(int64_t);
    printf("   Allocating %.2f MB... ", bytes_needed / (1024.0 * 1024.0));
    fflush(stdout);

    int64_t *array = (int64_t *)malloc(bytes_needed);

    if (!array)
    {
        printf("FAILED\n");
        fprintf(stderr, "Memory allocation failed (requested %.2f MB)\n",
                bytes_needed / (1024.0 * 1024.0));
        fprintf(stderr, "Try: ulimit -d unlimited\n");
        return -1.0;
    }

    // Touch the memory to ensure it's actually allocated
    memset(array, 0, bytes_needed);
    printf("OK\n");
    fflush(stdout);

    clock_gettime(CLOCK_MONOTONIC, &start);

    // Sequential write
    for (int64_t ii = 0; ii < ARRAY_SIZE; ii++)
        array[ii] = ii * 2;

    // Sequential read and accumulate
    volatile int64_t sum = 0;
    for (int64_t ii = 0; ii < ARRAY_SIZE; ii++)
        sum += array[ii];

    // Random access pattern
    for (int64_t ii = 0; ii < ARRAY_SIZE / 10; ii++)
    {
        int64_t idx = (ii * 7919LL) % ARRAY_SIZE;
        array[idx] = array[idx] * 2 + 1;
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    free(array);

    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1e9;
    return elapsed;
}

void print_system_info(void)
{
    struct utsname sys_info;
    if (uname(&sys_info) == 0)
    {
        printf("System Information:\n");
        printf("  OS: %s\n", sys_info.sysname);
        printf("  Node: %s\n", sys_info.nodename);
        printf("  Release: %s\n", sys_info.release);
        printf("  Machine: %s\n", sys_info.machine);
        printf("\n");
    }
}

// gcc -Wall -Wextra -Wpedantic -g -std=c99 perf-metrics.c -lm
int main(void)
{
    printf("=== CPU Benchmark Tool ===\n\n");
    print_system_info();
    printf("Running benchmarks...\n\n");
    double acc_score = 0.0;
    double acc_time = 0.0;
    int ii = 0;

    // repeat 5 times the test
    for (ii = 0; ii < 5; ii++)
    {
        printf("=== Test %d ===\n", ii + 1);
        //  Run integer operations benchmark
        printf("1. Integer Operations (%d iterations)...\n", ITERATIONS);
        double int_time = benchmark_int_ops();
        printf("   Time: %.4f seconds\n\n", int_time);

        // Run floating point benchmark
        printf("2. Floating Point Operations (%d iterations)...\n", ITERATIONS);
        double float_time = benchmark_float_ops();
        printf("   Time: %.4f seconds\n\n", float_time);

        // Run memory operations benchmark
        printf("3. Memory Operations (%d elements)...\n", ARRAY_SIZE);
        double mem_time = benchmark_memory_ops();
        printf("   Time: %.4f seconds\n\n", mem_time);

        // Calculate overall score (lower is better)
        double total_time = int_time + float_time + mem_time;
        acc_time += total_time;

        // Normalized score
        double score = 10000.0 / total_time;
        acc_score += score;

        printf("Total Time: %.4f seconds\n", total_time);
        printf("CPU Score: %.2f (higher is better)\n", score);
        printf("\n");
    }

    printf("=== Results ===\n");
    double avg_total_time = acc_time / ii;
    printf("Avg Total Time: %.4f seconds\n", avg_total_time);

    double avg_score = acc_score / ii;
    printf("Avg CPU Score: %.2f (higher is better)\n", avg_score);

    return 0;
}