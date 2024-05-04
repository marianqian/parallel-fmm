sequential:
cc -o fmm2d fmm2d.cpp -lm
./fmm2d

openmp: 
gcc -fopenmp -o fmm2domp fmm2d_omp.cpp -lm -lstdc++
./fmm2domp
