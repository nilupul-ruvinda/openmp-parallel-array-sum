# openmp-serial-array-sum
gcc -O2 serial_sum.c -o serial_sum
./serial_sum

# openmp-parallel-array-sum
gcc -O2 -fopenmp parallel_sum.c -o parallel_sum
./parallel_sum

#adv ops

pthread_create() is a POSIX thread library function used to create a new thread in a C program. A thread is a separate flow of execution within a process.
It allows a program to perform multiple tasks concurrently rather than executing everything sequentially.

int pthread_create(
    pthread_t *thread,
    const pthread_attr_t *attr,
    void *(*start_routine)(void *),
    void *arg
);

Uses of pthread_create():
Creates new threads to perform tasks concurrently.
Improves application responsiveness.
Allows multiple operations to execute at the same time.
Makes better use of multicore processors.


pthread_join() is a POSIX thread library function used to wait for a specific thread to finish its execution.
It prevents the main thread from continuing before the specified thread has completed its task.

int pthread_join(pthread_t thread, void **retval);

Uses of pthread_join():
Waits until a particular thread finishes execution.
Ensures that the main program does not terminate before its threads complete.
Allows the main thread to collect the result returned by another thread.
Helps prevent problems caused by accessing incomplete results.


The given program calculates the sum of all integers from 0 up to a number entered by the user.
For example, if the user enters 10, the program calculates:
0
+
1
+
2
+
3
+
⋯
+
10
=
55
0+1+2+3+⋯+10=55

How the program works:
It includes the standard input/output and standard library header files.
A global variable sum is initialized to zero.
The sum_runner() function receives a pointer to the upper limit.
The function uses a for loop to add all numbers from 0 to the given limit.
The main() function checks whether the user entered a command-line argument.
It converts the argument into a long long integer using atoll().
It calls sum_runner() to calculate the sum.
Finally, it displays the calculated sum.

gcc sum.c -o sum
./sum 10 ----------------------> output: Sum is 55


Input      Expected output
10,000      50,005,000
1,000,000    500,000,500,000
100,000,000    5,000,000,050,000,000
1,000,000,000    500,000,000,500,000,000
10,000,000,000    50,000,000,005,000,000,000

How to measure response time

time ./sum 10000
time ./sum 1000000
time ./sum 100000000
time ./sum 1000000000
time ./sum 10000000000

output
Sum is 50005000

real    0m0.002s
user    0m0.001s
sys     0m0.001s

-------->>> actual response times will depend on your computer's processor, operating system, and system load.


We can improve application responsiveness by dividing the calculation into smaller tasks and assigning each task to a separate thread.
Instead of one thread calculating the entire sum, multiple threads can calculate different portions simultaneously.

For example:
Thread 1 calculates the sum from 0 to 2,500.
Thread 2 calculates the sum from 2,501 to 5,000.
Thread 3 calculates the sum from 5,001 to 7,500.
Thread 4 calculates the sum from 7,501 to 10,000.

After all threads finish, the main thread combines their results.

This technique is called multithreading.


The following program divides the calculation between four threads.


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 4

typedef struct {
    long long start;
    long long end;
    __int128 result;
} ThreadData;

void *sum_runner(void *arg)
{
    ThreadData *data = (ThreadData *)arg;

    data->result = 0;

    for (long long i = data->start; i <= data->end; i++)
    {
        data->result += i;
    }

    return NULL;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Usage: %s <num>\n", argv[0]);
        return 1;
    }

    long long limit = atoll(argv[1]);

    pthread_t threads[NUM_THREADS];
    ThreadData data[NUM_THREADS];

    long long chunk = (limit + 1) / NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; i++)
    {
        data[i].start = i * chunk;
        data[i].end = (i == NUM_THREADS - 1)
                      ? limit
                      : (i + 1) * chunk - 1;

        pthread_create(&threads[i], NULL,
                       sum_runner, &data[i]);
    }

    __int128 total = 0;

    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
        total += data[i].result;
    }

    // Print the __int128 result
    char buffer[50];
    int pos = 49;
    buffer[pos] = '\0';

    if (total == 0)
        buffer[--pos] = '0';

    while (total > 0)
    {
        buffer[--pos] = '0' + total % 10;
        total /= 10;
    }

    printf("Sum is %s\n", &buffer[pos]);

    return 0;
}

compile:
gcc pthread_sum.c -o pthread_sum -pthread

run:
./pthread_sum 10000
./pthread_sum 1000000
./pthread_sum 100000000
./pthread_sum 1000000000
./pthread_sum 10000000000

Explanation of the modified program:

NUM_THREADS defines the number of threads as four.
ThreadData stores the starting value, ending value, and partial sum for each thread.
pthread_t threads[] stores the IDs of the created threads.
The input range is divided into four smaller ranges.
pthread_create() creates four threads to calculate their assigned ranges.
Each thread calculates its partial sum independently.
pthread_join() waits until all four threads finish.
The main thread combines the partial sums and displays the final result.

The program uses __int128 to handle the larger result that cannot fit inside a normal long long variable. This is a GCC extension rather than a standard C data type.

Important: Multithreading does not guarantee that every program will run faster. For smaller inputs, creating and managing threads can introduce additional overhead. Also, the loop-based approach still performs billions of additions for very large inputs.


Is the output for 10,000,000,000 correct?
No. The original program does not produce the correct result for this input because of integer overflow.
the program uses:
long long sum = 0;
This (10,000,000,000) value exceeds the maximum value of a signed long long.


When the pthread_join() statement is removed, the main thread does not wait for the created threads to finish their calculations.

As a result:
The main thread may continue executing before the worker threads finish.
The program may display an incomplete result.
The main thread may terminate before the worker threads complete.
The output may become unpredictable.


pthread_join() is important because it synchronizes the execution of threads.
for (int i = 0; i < NUM_THREADS; i++)
{
    pthread_join(threads[i], NULL);
}

This ensures that all four worker threads finish their calculations before the main thread combines their results.

Without pthread_join(), the main thread could read the partial results before they have been calculated.

Therefore, pthread_join() is necessary to ensure that the program waits for its worker threads and obtains the correct final result.


#Conclusion
how to create and manage threads using the POSIX thread library in C.

We explored two important functions:

pthread_create() creates new threads to execute tasks concurrently.

pthread_join() waits for threads to finish their execution.

modified a sequential summation program into a multithreaded program. This demonstrated how a large task can be divided among multiple threads and how their individual results can be combined.

integer overflow and the importance of thread synchronization when developing multithreaded applications.

the four-thread version is intended to demonstrate parallel processing; it may not be faster than the original for every input size.
