/******************************************************************************
 * main.cc
 *
 * Parallel Memory Bandwidth Measurement Tool.
 *
 ******************************************************************************
 * Copyright (C) 2013 Timo Bingmann <tb@panthema.net>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <vector>

#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include <omp.h>

// *** Global Settings

const char* g_funcfilter;
size_t g_sizelimit = 4*1024*1024*1024LLU;  // default size limit: 4 GiB
int g_nprocs_min = 0;                      // lower limit to number of threads
int g_nprocs_max = 0;                      // upper limit to number of threads

bool g_testcycle = false; // option to test permutation cycle before measurement

#define ERR(x)  do { std::cerr << x << std::endl; } while(0)
#define ERRX(x)  do { std::cerr << x; } while(0)

// *** Registry for memory testing function

typedef void (*testfunc_type)(void* memarea, size_t size, size_t repeats);

struct TestFunction
{
    // identifier of the test function
    const char* name;

    // function to call
    testfunc_type func;

    // number of bytes read/written per access (for latency calculation)
    unsigned int bytes_per_access;

    // bytes skipped foward to next access point (including bytes_per_access)
    unsigned int access_offset;

    // fill the area with a permutation before calling the func
    bool make_permutation;

    // constructor which also registers the function
    TestFunction(const char* _name, testfunc_type _func,
                 unsigned int _bpa, unsigned int _ao, bool _mp);
};

std::vector<TestFunction*> g_testfunc_list;

TestFunction::TestFunction(const char* _name, testfunc_type _func,
                           unsigned int _bpa, unsigned int _ao, bool _mp)
    : name(_name), func(_func),
      bytes_per_access(_bpa), access_offset(_ao), make_permutation(_mp)
{
    g_testfunc_list.push_back(this);
}

#define REGISTER(func, bytes, offset)                           \
    static const class TestFunction* _##func##_register =       \
        new TestFunction(#func,func,bytes,offset,false);

#define REGISTER_PERM(func, bytes)                              \
    static const class TestFunction* _##func##_register =       \
        new TestFunction(#func,func,bytes,bytes,true);

// *** Test Functions with inline assembler loops

#include "funcs_x86_64.h"

// *** Main Program

/// parse a number as size_t with error detection
static inline bool
parse_sizet(const char* value, size_t& out)
{
    char* endp;
    out = strtoull(value, &endp, 10);
    return (endp && *endp == 0);
}

/// parse a number as int with error detection
static inline bool
parse_int(const char* value, int& out)
{
    char* endp;
    out = strtoul(value, &endp, 10);
    return (endp && *endp == 0);
}

/// Simple linear congruential random generator
struct LCGRandom
{
    size_t      xn;

    inline LCGRandom(size_t seed) : xn(seed) { }

    inline size_t operator()()
    {
        xn = 0x27BB2EE687B0B0FDLLU * xn + 0xB504F32DLU;
        return xn;
    }
};

/// Create a one-cycle permutation of pointers in the memory area
void make_cyclic_permutation(void* memarea, size_t bytesize)
{
    void** ptrarray = (void**)memarea;
    size_t size = bytesize / sizeof(void*);

#pragma omp single
    (std::cout << "Make permutation:").flush();

    (std::cout << " filling").flush();

    for (size_t i = 0; i < size; ++i)
    {
        ptrarray[i] = &ptrarray[i];       // fill area with pointers to self-address
    }

    (std::cout << " permuting").flush();

    LCGRandom srnd((size_t)ptrarray + 233349568);

    for (size_t n = size; n > 1; --n)
    {
        size_t i = srnd() % (n-1);      // permute pointers to one-cycle
        std::swap( ptrarray[i], ptrarray[n-1] );
    }

    if (g_testcycle)
    {
        (std::cout << " testing").flush();

        void* ptr = ptrarray[0];
        size_t steps = 1;

        while ( ptr != &ptrarray[0] && steps < size*2 )
        {
            ptr = *(void**)ptr;         // walk pointer
            ++steps;
        }
        (std::cout << " cycle=" << steps).flush();

        assert(steps == size);
    }
    else
    {
        (std::cout << " cycle=" << size).flush();
    }

#pragma omp barrier
#pragma omp single
    std::cout << std::endl;
}

/// List of array sizes to test
static const size_t areasize_list[] = {
    1 * 1024,                   // 1 KiB
    2 * 1024,
    3 * 1024,
    4 * 1024,
    6 * 1024,
    8 * 1024,
    12 * 1024,
    16 * 1024,
    20 * 1024,
    24 * 1024,
    28 * 1024,
    32 * 1024,
    40 * 1024,
    48 * 1024,
    64 * 1024,
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
    2048 * 1024,                // 2 MiB = common L2 cache size
    (2048 + 256) * 1024,	// 2.25
    (2048 + 512) * 1024,	// 2.5
    (2048 + 768) * 1024,	// 2.75
    3 * 1024 * 1024,            // 3 MiB = common L2 cache size
    4 * 1024 * 1024,            // 4 MiB
    5 * 1024 * 1024,            // 5 MiB
    6 * 1024 * 1024,            // 6 MiB = common L2 cache size
    7 * 1024 * 1024,            // 7 MiB
    8 * 1024 * 1024,            // 8 MiB = common L2 cache size
    9 * 1024 * 1024,
    10 * 1024 * 1024,
    12 * 1024 * 1024,
    14 * 1024 * 1024,
    16 * 1024 * 1024,
    20 * 1024 * 1024,
    24 * 1024 * 1024,
    28 * 1024 * 1024,
    32 * 1024 * 1024,
    64 * 1024 * 1024,
    128 * 1024 * 1024,
    256 * 1024 * 1024,
    512 * 1024 * 1024,
    1 * 1024 * 1024 * 1024LLU,          // 1 GiB
    2 * 1024 * 1024 * 1024LLU,
    4 * 1024 * 1024 * 1024LLU,
    8 * 1024 * 1024 * 1024LLU,
    16 * 1024 * 1024 * 1024LLU,
    32 * 1024 * 1024 * 1024LLU,
    64 * 1024 * 1024 * 1024LLU,
    128 * 1024 * 1024 * 1024LLU,
    256 * 1024 * 1024 * 1024LLU,
    512 * 1024 * 1024 * 1024LLU,
    1024 * 1024 * 1024 * 1024LLU,       // 1 TiB
    0   // list termination
};

void testfunc_proc(char* memarea, const size_t memsize, const TestFunction* func, int nprocs)
{
    size_t factor = 1024*1024*1024; // repeat factor, approximate B/s bandwidth

    for (const size_t* areasize = areasize_list; *areasize; ++areasize)
    {
        if (*areasize > g_sizelimit && g_sizelimit != 0) {
            ERR("Skipping " << func->name << " test with " << *areasize << " array size due to -s <size limit>.");
            continue;
        }

        for (unsigned int round = 0; round < 1; ++round)
        {
            size_t thrsize = *areasize / nprocs;            // divide area by processor number

            // unrolled tests do 16 accesses without loop check, thus align upward
            // to next multiple of 16*size (128 bytes for 64-bit and 256 bytes for 128-bits)
            size_t unrollsize = 16 * func->bytes_per_access;
            thrsize = ((thrsize + unrollsize) / unrollsize) * unrollsize;

            size_t testsize = thrsize * nprocs;             // total size tested
            if (memsize < testsize) continue;               // skip if tests don't fit into memory

            // due to cache thrashing in adjacent cache lines, space out processor's test areas
            //size_t thrsize_spaced = std::max<size_t>(thrsize, 32*1024*1024 + 4096);
            size_t thrsize_spaced = std::max<size_t>(thrsize, 4*1024*1024 + 16*1024);
            if (memsize < thrsize_spaced * nprocs) continue;        // skip if tests don't fit into memory

            size_t repeats = (factor + thrsize-1) / thrsize;         // round up

            // volume in bytes tested
            size_t testvol = testsize * repeats * func->bytes_per_access / func->access_offset;
            // number of accesses in test
            size_t testaccess = testsize * repeats / func->access_offset;

            ERR("Running"
                << " nprocs=" << nprocs
                << " factor=" << factor
                << " areasize=" << *areasize
                << " thrsize=" << thrsize
                << " testsize=" << testsize
                << " repeats=" << repeats
                << " testvol=" << testvol
                << " testaccess=" << testaccess);

            double runtime;

#pragma omp parallel num_threads(nprocs)
            {
                // create cyclic permutation for each processor
                if (func->make_permutation)
                    make_cyclic_permutation(memarea + omp_get_thread_num() * thrsize_spaced, thrsize);

#pragma omp barrier
                double ts1 = omp_get_wtime();
                func->func(memarea + omp_get_thread_num() * thrsize_spaced, thrsize, repeats);

#pragma omp barrier
                double ts2 = omp_get_wtime();
#pragma omp master
                runtime = ts2 - ts1;
            }

            if ( runtime < 1.0 )
            {
                // test ran for less than one second, repeat test and scale repeat factor
                factor = thrsize * repeats * 3/2 / runtime;
                ERR("run time = " << runtime << " -> rerunning test with repeat factor=" << factor);

                --round;     // redo this areasize
            }
            else
            {
                // adapt repeat factor to observed memory bandwidth, so that next test will take approximately 1.5 sec

                factor = thrsize * repeats * 3/2 / runtime;
                ERR("run time = " << runtime << " -> next test with repeat factor=" << factor);

                std::ostringstream result;
                result << "RESULT\t";

                // output date, time and hostname to result line
                char datetime[64];
                time_t tnow = time(NULL);

                strftime(datetime, sizeof(datetime), "%Y-%m-%d %H:%M:%S", localtime(&tnow));
                result << "datetime=" << datetime << "\t";

                char hostname[256];
                gethostname(hostname, sizeof(hostname));
                result << "host=" << hostname << "\t";

                result << "funcname=" << func->name << "\t"
                       << "nprocs=" << nprocs << "\t"
                       << "areasize=" << *areasize << "\t"
                       << "threadsize=" << thrsize << "\t"
                       << "testsize=" << testsize << "\t"
                       << "repeats=" << repeats << "\t"
                       << "testvol=" << testvol << "\t"
                       << "testaccess=" << testaccess << "\t"
                       << "time=" << std::setprecision(20) << runtime << "\t"
                       << "bandwidth=" << testvol / runtime << "\t"
                       << "rate=" << runtime / testaccess;

                std::cout << result.str() << std::endl;

                std::ofstream resultfile("stats.txt", std::ios::app);
                resultfile << result.str() << std::endl;
            }
        }
    }
}

void testfunc(char* memarea, const size_t memsize, const TestFunction* func)
{
    if (g_funcfilter && strstr(func->name, g_funcfilter) == NULL) {
        ERR("Skipping " << func->name << " tests");
        return;
    }

    const int maxprocs = omp_get_num_procs();

    for (int nprocs = 1; nprocs <= maxprocs+2; ++nprocs)
    {
        if ( nprocs < g_nprocs_min || (g_nprocs_max && nprocs > g_nprocs_max) ) {
            ERR("Skipping " << func->name << " tests with " << nprocs << " threads.");
            continue;
        }

        testfunc_proc(memarea, memsize, func, nprocs);
    }
}

static inline size_t round_up_power2(size_t v)
{
    v--;
    v |= v >> 1;     v |= v >> 2;
    v |= v >> 4;     v |= v >> 8;
    v |= v >> 16;    v |= v >> 32;
    v++;
    return v + (v == 0);
}

int main(int argc, char* argv[])
{
    // *** parse command line options

    int opt;

    while ( (opt = getopt(argc, argv, "f:s:p:P:")) != -1 )
    {
        switch (opt) {
        case 'f':
            g_funcfilter = optarg;

            if (strcmp(g_funcfilter,"list") == 0)
            {
                std::cout << "Test Function List" << std::endl;
                for (size_t i = 0; i < g_testfunc_list.size(); ++i)
                    std::cout << "  " << g_testfunc_list[i]->name << std::endl;
                return 0;
            }

            ERR("Running only functions containing '" << g_funcfilter << "'");
            break;

        case 's':
            if (!parse_sizet(optarg, g_sizelimit)) {
                ERR("Invalid parameter for -s <size limit>.");
                exit(EXIT_FAILURE);
            }
            else if (g_sizelimit == 0) {
                ERR("Running test with array without size limit.");
            }
            else {
                ERR("Running tests with array up to size " << g_sizelimit << ".");
            }
            break;

        case 'p':
            if (!parse_int(optarg, g_nprocs_min)) {
                ERR("Invalid parameter for -p <lower nprocs limit>.");
                exit(EXIT_FAILURE);
            }
            else {
                ERR("Running tests with at least " << g_nprocs_min << " threads.");
            }
            break;

        case 'P':
            if (!parse_int(optarg, g_nprocs_max)) {
                ERR("Invalid parameter for -p <upper nprocs limit>.");
                exit(EXIT_FAILURE);
            }
            else {
                ERR("Running tests with up to " << g_nprocs_max << " threads.");
            }
            break;

        default: /* '?' */
            ERR("Usage: " << argv[0] << " [-f funcfilter] [-s size limit] [-p min threads] [-P max threads]");
            exit(EXIT_FAILURE);
        }
    }

    // *** allocate memory for tests

    size_t physical_mem = sysconf(_SC_PHYS_PAGES) * (size_t)sysconf(_SC_PAGESIZE);

    // round down memory to largest power of two, still fitting in physical RAM
    size_t memsize = round_up_power2(physical_mem) / 2;

    // due to roundup in loop to next cache-line size, add one extra cache-line per processor
    memsize += omp_get_num_procs() * 256;

    ERR("Detected " << physical_mem / 1024/1024 << " MiB physical RAM and " << omp_get_num_procs() << " CPUs. " << std::endl
        << "Allocating " << memsize / 1024/1024 << " MiB for testing.");

    // allocate memory area
    char* memarea = (char*)malloc(memsize);

    // fill memory with junk, but this allocates physical memory
    memset(memarea, 1, sizeof(memarea));

    // *** perform memory tests

    unlink("stats.txt");

    for (size_t i = 0; i < g_testfunc_list.size(); ++i)
    {
        testfunc(memarea, memsize, g_testfunc_list[i]);
    }

    free(memarea);

    return 0;
}
