# parallel-computing
 General information:    
MPI (message passage interface) in parallel computing.
1) This program aims to calculate simple Matrices multiplication in two
ways(sequential & parallel).
then, compute the checksum(data integrity) & execution time(speedup) in two
cases. then, make comparison between them.
2) We tested different array sizes, like: 128, 256, 512 & 1024 (we attached
screenshots) :
- in 128 , the parallel speedup was almost 2 time more than sequential .
- in 256 , the parallel speedup was almost 3 time more than sequential .
- in 512 , the parallel speedup was almost 4 time more than sequential .
- in 1024, the parallel speedup was almost 13 time more than sequential .
3) we found that Checksum authenticate our result that we got in the parallel
Coding comparing with Checksum of sequential code (that means: our results
is true).
4) We also conclude that Checksum still have the same result in two ways (Row
Wise or column wise); Because we depend on checksum with XOR algorithm
which apply XOR for all matrix result elements.
5) For measuring Wall clock time we have used MPI_Wtime() function, which is
include in MPI libraries. MPI_Wtime() Returns an elapsed time on the calling
processor and the Return value Time is in seconds since an arbitrary time in the
past.in Seconds.
 
