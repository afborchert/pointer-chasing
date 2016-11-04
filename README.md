# pointer-chasing
Utilities to measure read access times of caches, memory, and hardware prefetches for simple and fused operations

## Summary

This package provides the following three utilities:

* _random-chase_: measure average read access times of all cache
  levels and main memory
* _linear-chase_: measure read access times for a linear access
  pattern with a constant stride
* _fused-linear-chase_: like _linear-chase_ but for an interleaved
  access pattern of multiple linear sequences, all with the same stride

All of them work with memory buffers that are organized as an array
of pointers where
* all pointers point into the very same buffer, and where
* beginning from any pointer all other pointers can be reached
  following the pointer chain, and where
* all locations are reached.

Once such a memory buffer has been set up, we measure the time of

```C
void** p = (void**) memory[0];
while (count-- > 0) {
   p = (void**) *p;
}
```

The `p = (void**) *p` construct enforces all memory accesses to be
serialized, i.e. the next access can only be scheduled by the processor
when the previous fetch has been finished. To defeat optimizers who
tend to optimize the loop away when the result isn't used, the last
pointer value is assigned to a `volatile` global variable which is
otherwise unused.

In case of _fused-linear-chase_ multiple such buffers are configured
in dependence of the fuse factor.

For the sake of simplicity, all utilities are parameterized through
preprocessor macros.

The idea of pointer chasing is not new, in fact there exist quite
a number of papers and other utilities related to it.

## random-chase

Following preprocessor macros allow to configure this utility:

* *MIN_SIZE*: Minimal buffer size in bytes which should be small enough
  to fit comfortably into the L1 cache.
* *MAX_SIZE*: Maximal buffer size in bytes which should be larger than
  the L3 cache.
* *GRANULARITY*: All powers of two between *MIN_SIZE* and *MAX_SIZE*
  are tested. The granularity specifies how many sizes are tested
  in-between. For a granularity of _n_ we get _2^n_ sizes in-between.

The output consists of a header line and then a line for each tested
buffer size from *MIN_SIZE* to *MAX_SIZE* where the memory size and
the measured access time in nanoseconds is given.

This is the sample output for an Intel Xeon 5650 with three caches
(L1: 32 KiB, L2: 256 KiB, L3: 12 MiB) with default
parameters, i.e. *MIN_SIZE* = 1024, *MAX_SIZE* = 32 MiB, and
*GRANULARITY* = 1:

```
  memsize  time in ns
     1024     1.36904
     1536     1.35973
     2048     1.36904
     3072     1.35973
     4096     1.35973
     6144     1.36904
     8192     1.35973
    12288     1.35973
    16384     1.35973
    24576     1.37836
    32768     1.36904
    49152     2.06754
    65536     2.43075
    98304     2.79397
   131072     2.97092
   196608     3.14787
   262144     3.29688
   393216     8.37259
   524288    10.85922
   786432    13.20615
  1048576    14.38893
  1572864    15.59965
  2097152    16.23295
  3145728    18.08628
  4194304    18.34705
  6291456    19.79060
  8388608    22.66839
 12582912    31.55321
 16777216    43.27856
 25165824    59.39975
 33554432    64.04705
```

A gnuplot script may be helpful to visualize this:

```gnuplot
set terminal png size 900, 500
set output "random-chase.png"
set xlabel "memory area in bytes"
set logscale x
set ylabel "avg access time in ns"
set title "Access times in dependence of memory area"
set key out
set pointsize 0.5

# determine maximal y value by plotting to a dummy terminal
set terminal push
set terminal unknown
plot "random-chase.out" using 2
set terminal pop

# mark L1, L2, and L3:
maxy = GPVAL_Y_MAX
l1 = 32
l2 = 256
l3 = 12288
set arrow from l1*1024,0 to l1*1024,maxy nohead lc rgb 'blue';
set arrow from l2*1024,0 to l2*1024,maxy nohead lc rgb 'blue';
set arrow from l3*1024,0 to l3*1024,maxy nohead lc rgb 'blue';

plot "random-chase.out" using 1:2 with linespoints lt 2 title "Intel Xeon 5650"
```

Result:

![Memory access times with random chain](random-chase.png)

## linear-chase

Following preprocessor macros configure this utility:

* *MIN_STRIDE*: Minimal stride value. By default, `sizeof(void*)`
  is taken.
* *MAX_STRIDE*: Maximal stride value.

The output consists of a header line and then a line for each
tested stride value from *MIN_STRIDE* to *MAX_STRIDE* in
steps of `sizeof(void*)` and the measured acess time in
nanoseconds.

This is a sample output for the same Intel Xeon 5650
compiled for an 32-bit address space, i.e. `sizeof(void*) == 4`
which has been shortened for brevity:

```
   stride  time in ns
        4     1.30385
        8     1.31316
       12     1.31316
       ...
     1188    13.57868
     1192    13.10371
     1196    13.52280
     1200    13.58800
```

A gnuplot script may be helpful as before:

```gnuplot
set terminal png size 900, 500
set output "linear-chase.png"
set xlabel "stride in bytes"
set ylabel "avg access time in ns"
set title "Access times in dependence of stride"
set key out
set pointsize 0.5

plot "linear-chase.out" using 1:2 with linespoints lt 2 title "Intel Xeon 5650"
```

Result:

![Memory access times in dependence of stride](linear-chase.png)

## Downloading and testing

If you want to clone this project, you should do this recursively:

```
git clone --recursive https://github.com/afborchert/pointer-chasing.git
```

To build it, just invoke make. You need g++ supporting C++11 and GNU make
for this to work.
