# pmbw - Parallel Memory Bandwidth Measurement / Benchmark

The tool **`pmbw`** is a set of assembler routines to measure the **parallel
memory (cache and RAM) bandwidth** of modern multi-core machines. Memory
bandwidth is one of the key performance factors of any computer system. And
today, measuring the memory performance often gives a **more realistic view**
on the overall speed of a machine **than pure arithmetic or floating-point
benchmarks**. This is due to the speed of computation units in modern CPUs
growing faster than the memory bandwidth, which however is required to get more
information to the CPU. The bigger the processed data amount gets, the more
important memory bandwidth becomes!

The **`pmbw`** tool contains a set of very basic functions, which are all
**hand-coded in assembler** to avoid any compiler optimizations. These basic
functions are modeled after the **basic inner loops** found in any data
processing: **sequential scanning** and **pure random access**. Any application
will have a memory access pattern that is somewhere between these two extremes.

Besides these two access patterns, the basic functions benchmark different
modes of memory access. Depending on the architecture, **16- / 32- / 64- / 128-
or 256-bit memory transfers** are tested by using different machine
instructions, like MMX, SSE or AVX. Furthermore, iterating by pointers is
compared against access via array index. The current version of `pmbw` supports
benchmarking **x86_32-bit**, **x86_64-bit** and **ARMv6** systems..

Most important feature of this benchmark is that it will perform the tests **in
parallel with growing number of threads**. The results of these scalability
tests highlight the basic problem which parallel multi-core algorithms must
cope with: **scanning memory bandwidth does not scale** with the number of
cores in current systems. The **ratio** of bandwidth **to cache** over the
bandwidth **to RAM** determines the amount of local cache-based processing
which must be done between RAM accesses for an algorithm to scale well.

## Website and License

The current source package and some binaries can be downloaded from
http://panthema.net/2013/pmbw/

We also collect results from various multi-core systems on the page above.

The program and code is published under the GNU General Public License v3
(GPL), which can also be found in the file COPYING.

## Exits

The basic idea of measuring memory bandwidth is not new, however, none of the
existing benchmarks target multi-core parallelism, growing array sizes and
simple program loops. The [STREAM benchmark](http://www.streambench.org/)
allows tuning for specific hardware and is not in assembler
code. [Zack Smith's bandwidth](http://zsmith.co/bandwidth.html) benchmark is
limited to sequential bandwidth and was the starting point for designing
`pmbw`.

Written 2013-07-08 by Timo Bingmann
