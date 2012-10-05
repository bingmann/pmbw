#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <omp.h>

#include "statsfile.h"

extern "C" {

void funcFill(void* memarea, size_t size);

void funcSeqWrite64PtrSimpleLoop(void* memarea, size_t size, size_t repeats);
void funcSeqWrite64PtrUnrollLoop(void* memarea, size_t size, size_t repeats);
void funcSeqRead64PtrSimpleLoop(void* memarea, size_t size, size_t repeats);
void funcSeqRead64PtrUnrollLoop(void* memarea, size_t size, size_t repeats);
void funcSeqWrite128PtrSimpleLoop(void* memarea, size_t size, size_t repeats);
void funcSeqWrite128PtrUnrollLoop(void* memarea, size_t size, size_t repeats);
void funcSeqRead128PtrSimpleLoop(void* memarea, size_t size, size_t repeats);
void funcSeqRead128PtrUnrollLoop(void* memarea, size_t size, size_t repeats);
void funcSeqWrite64IndexSimpleLoop(void* memarea, size_t size, size_t repeats);
void funcSeqWrite64IndexUnrollLoop(void* memarea, size_t size, size_t repeats);
void funcSeqRead64IndexSimpleLoop(void* memarea, size_t size, size_t repeats);
void funcSeqRead64IndexUnrollLoop(void* memarea, size_t size, size_t repeats);
void funcSkipWrite64PtrSimpleLoop(void* memarea, size_t size, size_t repeats);
void funcSkipRead64PtrSimpleLoop(void* memarea, size_t size, size_t repeats);
void funcSkipWrite128PtrSimpleLoop(void* memarea, size_t size, size_t repeats);
void funcSkipRead128PtrSimpleLoop(void* memarea, size_t size, size_t repeats);
void funcSkipWrite64IndexSimpleLoop(void* memarea, size_t size, size_t repeats);
void funcSkipRead64IndexSimpleLoop(void* memarea, size_t size, size_t repeats);

void funcPermRead64SimpleLoop(void* memarea, size_t dummy, size_t repeats);
void funcPermRead64UnrollLoop(void* memarea, size_t dummy, size_t repeats);
}

void testfunc_nonthreaded( void (*func)(void* memarea, size_t size, size_t repeats), const char* funcname, int datasize, int skiplen )
{
    size_t memsize = 2*1024*1024*1024LLU;

    void* memarea = malloc(memsize);
    funcFill(memarea, memsize);

    for (size_t size = 1024; size <= memsize; size *= 2)
    {
        size_t vol = memsize * 2;
        unsigned int repeats = vol / size * (skiplen*8/datasize);

        double ts1 = omp_get_wtime();
        func(memarea, size, repeats);
        double ts2 = omp_get_wtime();

        StatsWriter sw("stats.txt");
        sw >> "funcname" << funcname
           >> "areasize" << size
           >> "time" << (ts2-ts1)
           >> "speed" << vol / (ts2-ts1)
           >> "rate" << (ts2-ts1) / (vol / (datasize/8));

        std::cout << "size=" << size << "\ttime=" << ts2-ts1
                  << "\tspeed=" << std::fixed << vol / (ts2-ts1) / 1024/1024
                  << "\trate=" << std::fixed << (ts2-ts1) / vol * 1e9 << "\n";
    }

    free(memarea);
}

class LCGRandom
{
private:
    size_t      xn;

public:

    LCGRandom(size_t seed)
        : xn(seed)
    {
    }

    size_t operator()()
    {
        xn = 0x27BB2EE687B0B0FDLLU * xn + 0xB504F32DLU;
        return xn;
    }
};

void make_cyclic_pointer_permutation(void** memarea, size_t size)
{
    (std::cout << "Make permutation: filling").flush();

    for (size_t i = 0; i < size; ++i)
    {
        memarea[i] = &memarea[i];       // fill area with pointers to self-address
    }

    (std::cout << " permuting").flush();

    LCGRandom srnd((size_t)memarea + 23334956468);

    for (size_t n = size; n > 1; --n)
    {
        size_t i = srnd() % (n-1);        
        std::swap( memarea[i], memarea[n-1] );
    }

    (std::cout << " testing").flush();

    {
        void* ptr = memarea[0];
        size_t steps = 1;

        while ( ptr != &memarea[0] && steps < size*2 )
        {
            ptr = *(void**)ptr;
            ++steps;
        }
        std::cout << ", cycle = " << steps << "\n";
    }
}

void make_cyclic_permutation(void* memarea, size_t bytesize)
{
    return make_cyclic_pointer_permutation((void**)memarea, bytesize / sizeof(void*));
}

static const size_t testsize_list[] = {
    1024,
    2048,
    3072,
    4096,
    6144,
    8192,	// Some processors' L1 data caches are only 8kB.
    12288,
    16384,
    20480,
    24576,
    28672,
    32768,	// Common L1 data cache size.
    40960,
    49152,
    65536,
    128 * 1024,
    192 * 1024,
    256 * 1024,
    384 * 1024,
    512 * 1024,
    768 * 1024,
    1024 * 1024,                // 1 MiB
    (1024 + 256) * 1024,	// 1.25 MiB
    (1024 + 512) * 1024,	// 1.5 MiB
    (1024 + 768) * 1024,	// 1.75 MiB
    2048 * 1024,	        // 2 MiB = common L2 cache size
    (2048 + 256) * 1024,	// 2.25
    (2048 + 512) * 1024,	// 2.5
    (2048 + 768) * 1024,	// 2.75
    3 * 1024 * 1024,	        // 3 MiB = common L2 cache size
    4 * 1024 * 1024,            // 4 MiB
    5 * 1024 * 1024,    	// 5 MiB
    6 * 1024 * 1024,    	// 6 MiB = common L2 cache size
    7 * 1024 * 1024,    	// 7 MiB
    8 * 1024 * 1024,
    9 * 1024 * 1024,
    10 * 1024 * 1024,
    12 * 1024 * 1024,
    16 * 1024 * 1024,
    32 * 1024 * 1024,
    64 * 1024 * 1024,
    128 * 1024 * 1024,
    256 * 1024 * 1024,
    512 * 1024 * 1024,
    1 * 1024 * 1024 * 1024,     // 1 GiB
    2 * 1024 * 1024 * 1024LLU,  // 2 GiB
    4 * 1024 * 1024 * 1024LLU,  // 4 GiB
    8 * 1024 * 1024 * 1024LLU,  // 8 GiB
    0
};

void testfunc( void (*func)(void* memarea, size_t size, size_t repeats), const char* funcname,
               int access_size, int skiplen, bool use_permutation )
{
    const size_t memsize = 8*1024*1024*1024LLU;
    const int factor = 1;               // repeat factor

    int maxprocs = omp_get_num_procs();

    char* memarea = (char*)malloc(memsize + maxprocs * 64);

    if (!use_permutation) funcFill(memarea, memsize);

    for (int nprocs = 1; nprocs <= maxprocs+2; ++nprocs)
    {
#pragma omp parallel num_threads(nprocs)
        {
            for (const size_t* _testsize = testsize_list; *_testsize; ++_testsize)
            {
                size_t testsize = *_testsize;
                size_t thrsize = testsize / nprocs;
                thrsize = ((thrsize + 63) / 64) * 64;   // align upward to next multiple of 64

                size_t size = thrsize * nprocs;         // total size written
                size_t repeats = factor * memsize / testsize * (skiplen / access_size);

                size_t testvol = size * repeats * access_size / skiplen;        // volume in bytes tested
                size_t testaccess = size * repeats / skiplen;                   // number of accesses in test
/*
#pragma omp single
                printf("nprocs=%d  testsize=%llu  thrsize=%llu  size=%llu  repeats=%llu  testvol=%llu testaccess=%llu\n",
                       nprocs, testsize, thrsize, size, repeats, testvol, testaccess);
*/

                size_t thrsize2 = std::max<size_t>(thrsize, 64*1024*1024);

                // create cyclic permutation for each processor
                if (use_permutation)
                    make_cyclic_permutation(memarea + omp_get_thread_num() * thrsize2, thrsize);

#pragma omp barrier
                double ts1 = omp_get_wtime();
                func(memarea + omp_get_thread_num() * thrsize2, thrsize, repeats);

#pragma omp barrier
                double ts2 = omp_get_wtime();

#pragma omp single
                {
                    StatsWriter sw("stats.txt");
                    sw >> "funcname" << funcname
                       >> "nprocs" << nprocs
                       >> "testsize" << testsize
                       >> "thrsize" << thrsize
                       >> "size" << size
                       >> "repeats" << repeats
                       >> "testvol" << testvol
                       >> "testaccess" << testaccess
                       >> "time" << (ts2-ts1)
                       >> "bandwidth" << testvol / (ts2-ts1)
                       >> "rate" << (ts2-ts1) / testaccess;
                }
            }
        }
    }

    free(memarea);
}

#define TESTFUNC(x,bits,skip)           testfunc(x, #x, bits, skip, false)
#define TESTFUNC_PERM(x,bits,skip)      testfunc(x, #x, bits, skip, true)

int main()
{
    unlink("stats.txt");

    TESTFUNC( funcSeqWrite64PtrSimpleLoop, 8, 8 );
    TESTFUNC( funcSeqWrite64PtrUnrollLoop, 8, 8 );
    TESTFUNC( funcSeqRead64PtrSimpleLoop, 8, 8 );
    TESTFUNC( funcSeqRead64PtrUnrollLoop, 8, 8 );

    TESTFUNC( funcSeqWrite128PtrSimpleLoop, 16, 16 );
    TESTFUNC( funcSeqWrite128PtrUnrollLoop, 16, 16 );
    TESTFUNC( funcSeqRead128PtrSimpleLoop, 16, 16 );
    TESTFUNC( funcSeqRead128PtrUnrollLoop, 16, 16 );

    TESTFUNC( funcSeqWrite64IndexSimpleLoop, 8, 8 );
    TESTFUNC( funcSeqWrite64IndexUnrollLoop, 8, 8 );
    TESTFUNC( funcSeqRead64IndexSimpleLoop, 8, 8 );
    TESTFUNC( funcSeqRead64IndexUnrollLoop, 8, 8 );

    TESTFUNC( funcSkipWrite64PtrSimpleLoop, 8, 64+8 );
    TESTFUNC( funcSkipRead64PtrSimpleLoop, 8, 64+8 );
    TESTFUNC( funcSkipWrite128PtrSimpleLoop, 16, 64+16 );
    TESTFUNC( funcSkipRead128PtrSimpleLoop, 8, 64+16 );
    TESTFUNC( funcSkipWrite64IndexSimpleLoop, 8, 64+8 );
    TESTFUNC( funcSkipRead64IndexSimpleLoop, 8, 64+8 );

    TESTFUNC_PERM( funcPermRead64SimpleLoop, 8, 8 );
    TESTFUNC_PERM( funcPermRead64UnrollLoop, 8, 8 );

    return 0;
}
