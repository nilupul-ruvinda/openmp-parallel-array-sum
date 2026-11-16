#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 1000000000LL
#define NUM_THREADS 4

int main() {
    int *arr = (int *)malloc(N * sizeof(int));
    if (arr == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        return 1;
    }

    // Populate the array
    for (long long i = 0; i < N; i++) {
        arr[i] = (int)i;
    }

    double start, end;

    // ---------------------------------------------------------
    // Baseline: Serial Calculation (for correctness verification)
    // ---------------------------------------------------------
    long long serial_sum = 0;
    start = omp_get_wtime();
    for (long long i = 0; i < N; i++) {
        serial_sum += arr[i];
    }
    end = omp_get_wtime();
    printf("[Serial]               Sum: %lld | Time: %f s\n", serial_sum, end - start);

    // ---------------------------------------------------------
    // 2a - 2e: Parallel with Manual Thread-Local Sums
    // ---------------------------------------------------------
    long long manual_sum = 0;
    long long local_sums[NUM_THREADS] = {0};

    // 2a. Use four threads
    omp_set_num_threads(NUM_THREADS);

    start = omp_get_wtime();
    // 2b. Parallelize the for loop into four threads
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        long long private_sum = 0; // Register/stack-local avoids false sharing

        #pragma omp for
        for (long long i = 0; i < N; i++) {
            private_sum += arr[i];
        }

        // 2c. Store local sum for each thread
        local_sums[tid] = private_sum;
    }

    // 2d. Aggregate the local sums to get the final output
    for (int t = 0; t < NUM_THREADS; t++) {
        manual_sum += local_sums[t];
    }
    end = omp_get_wtime();
    // 2e. Measure time consumption
    printf("[OMP Manual Local]     Sum: %lld | Time: %f s\n", manual_sum, end - start);

    // ---------------------------------------------------------
    // 2f - 2g: Parallel with OpenMP Reduction Clause
    // ---------------------------------------------------------
    long long reduction_sum = 0;

    start = omp_get_wtime();
    #pragma omp parallel for num_threads(NUM_THREADS) reduction(+:reduction_sum)
    for (long long i = 0; i < N; i++) {
        reduction_sum += arr[i];
    }
    end = omp_get_wtime();
    printf("[OMP Reduction]        Sum: %lld | Time: %f s\n", reduction_sum, end - start);

    // ---------------------------------------------------------
    // 2g: Verify Correctness across all implementations
    // ---------------------------------------------------------
    if (reduction_sum == serial_sum && reduction_sum == manual_sum) {
        printf("\n[Verification PASSED] All outputs match: %lld\n\n", reduction_sum);
    } else {
        printf("\n[Verification FAILED] Mismatch in sums!\n\n");
    }

    // ---------------------------------------------------------
    // 2i: Reduction with Different Scheduling Policies
    // ---------------------------------------------------------
    long long static_sum = 0;
    start = omp_get_wtime();
    #pragma omp parallel for num_threads(NUM_THREADS) reduction(+:static_sum) schedule(static, 1000000)
    for (long long i = 0; i < N; i++) {
        static_sum += arr[i];
    }
    end = omp_get_wtime();
    printf("[OMP Schedule Static]  Sum: %lld | Time: %f s\n", static_sum, end - start);

    long long dynamic_sum = 0;
    start = omp_get_wtime();
    #pragma omp parallel for num_threads(NUM_THREADS) reduction(+:dynamic_sum) schedule(dynamic, 1000000)
    for (long long i = 0; i < N; i++) {
        dynamic_sum += arr[i];
    }
    end = omp_get_wtime();
    printf("[OMP Schedule Dynamic] Sum: %lld | Time: %f s\n", dynamic_sum, end - start);

    free(arr);
    return 0;
}
