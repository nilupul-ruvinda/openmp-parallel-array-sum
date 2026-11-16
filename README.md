# openmp-serial-array-sum
gcc -O2 serial_sum.c -o serial_sum
./serial_sum

# openmp-parallel-array-sum
gcc -O2 -fopenmp parallel_sum.c -o parallel_sum
./parallel_sum
