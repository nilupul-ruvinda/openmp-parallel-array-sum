#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1000000000LL

int main() {
    // a. Define an array with 1,000,000,000 elements on the heap (~4 GB)
    int *arr = (int *)malloc(N * sizeof(int));
    if (arr == NULL) {
        fprintf(stderr, "Memory allocation failed! Ensure at least 4GB RAM is free.\n");
        return 1;
    }

    // b. Populate the array
    for (long long i = 0; i < N; i++) {
        arr[i] = (int)i;
    }

    // d. Measure the time consumption for calculating the sum
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // c. Calculate the sum
    long long sum = 0;
    for (long long i = 0; i < N; i++) {
        sum += arr[i];
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) * 1e-9;

    printf("Serial Sum: %lld\n", sum);
    printf("Time taken: %f seconds\n", time_taken);

    free(arr);
    return 0;
}
