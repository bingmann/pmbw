/******************************************************************************
 * funcs_x86_64.h
 *
 * All 64-bit Test Functions: they are codename as 
 * Scan/Skip/Perm Read/Write 64 Ptr/Index Simple/Unroll Loop.
 *
 * Scan = consecutive scanning, Skip = with skipping bytes, Perm = walk
 * permutation cycle.
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

// 64-bit writer in a simple loop (C version)
void cScanWrite64PtrSimpleLoop(void* memarea, size_t size, size_t repeats)
{
    uint64_t* begin = (uint64_t*)memarea;
    uint64_t* end = begin + size / sizeof(uint64_t);
    uint64_t value = 0xC0FFEEEEBABE0000;

    do {
        uint64_t* p = begin;
        do {
            *p++ = value;
        }
        while (p < end);
    }
    while (--repeats != 0);
}

//REGISTER(cScanWrite64PtrSimpleLoop, 8, 8);

// 64-bit writer in a simple loop (Assembler version)
void ScanWrite64PtrSimpleLoopAdd(void* memarea, size_t size, size_t repeats)
{
    asm("mov    $0xC0FFEEEEBABE0000, %%rax \n" // rax = test value
        "mov    %[memarea], %%rsi \n"   // rsi = memarea
        "mov    %[size], %%rdi \n"              
        "add    %%rsi, %%rdi \n"        // rdi = memarea+size
        "1: \n" // start of repeat loop
        "mov    %%rsi, %%rcx \n"        // rcx = reset loop iterator
        "2: \n" // start of write loop
        "mov    %%rax, (%%rcx) \n"
        //"lea    8(%%rcx), %%rcx \n"
        "add    $8, %%rcx \n"
        // test write loop condition
        "cmp    %%rdi, %%rcx \n"        // compare to end iterator
        "jb     2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        :
        : [memarea] "g" (memarea), [size] "g" (size), [repeats] "r" (repeats)
        : "rax", "rcx", "rsi", "rdi");
}

REGISTER(ScanWrite64PtrSimpleLoopAdd, 8, 8);

// 64-bit writer in a simple loop (Assembler version)
void ScanWrite64PtrSimpleLoopLea(void* memarea, size_t size, size_t repeats)
{
    asm("mov    $0xC0FFEEEEBABE0000, %%rax \n" // rax = test value
        "mov    %[memarea], %%rsi \n"   // rsi = memarea
        "mov    %[size], %%rdi \n"              
        "add    %%rsi, %%rdi \n"        // rdi = memarea+size
        "1: \n" // start of repeat loop
        "mov    %%rsi, %%rcx \n"        // rcx = reset loop iterator
        "2: \n" // start of write loop
        "mov    %%rax, (%%rcx) \n"
        "lea    8(%%rcx), %%rcx \n"
        // test write loop condition
        "cmp    %%rdi, %%rcx \n"        // compare to end iterator
        "jb     2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        :
        : [memarea] "g" (memarea), [size] "g" (size), [repeats] "r" (repeats)
        : "rax", "rcx", "rsi", "rdi");
}

REGISTER(ScanWrite64PtrSimpleLoopLea, 8, 8);

// 64-bit writer in an unrolled loop (C version)
void cScanWrite64PtrUnrollLoop(void* memarea, size_t size, size_t repeats)
{
    uint64_t* begin = (uint64_t*)memarea;
    uint64_t* end = begin + size / sizeof(uint64_t);
    uint64_t value = 0xC0FFEEEEBABE0000;

    do {
        uint64_t* p = begin;
        do {
            *(p+0) = value;
            *(p+1) = value;
            *(p+2) = value;
            *(p+3) = value;
            *(p+4) = value;
            *(p+5) = value;
            *(p+6) = value;
            *(p+7) = value;
            *(p+8) = value;
            *(p+9) = value;
            *(p+10) = value;
            *(p+11) = value;
            *(p+12) = value;
            *(p+13) = value;
            *(p+14) = value;
            *(p+15) = value;
            p += 16;
        }
        while (p < end);
    }
    while (--repeats != 0);
}

//REGISTER(cScanWrite64PtrUnrollLoop, 8, 8);

// 64-bit writer in an unrolled loop (Assembler version)
void ScanWrite64PtrUnrollLoop(void* memarea, size_t size, size_t repeats)
{
    asm("mov    $0xC0FFEEEEBABE0000, %%rax \n"    // rax = test value
        "mov    %[memarea], %%rsi \n"   // rsi = memarea
        "mov    %[size], %%rdi \n"              
        "add    %%rsi, %%rdi \n"        // rdi = memarea+size
        "1: \n"                         // start of repeat loop
        "mov    %%rsi, %%rcx \n"        // rcx = reset loop iterator
        "2: \n"                         // start of write loop
        "mov    %%rax, 0*8(%%rcx) \n"
        "mov    %%rax, 1*8(%%rcx) \n"
        "mov    %%rax, 2*8(%%rcx) \n"
        "mov    %%rax, 3*8(%%rcx) \n"
        "mov    %%rax, 4*8(%%rcx) \n"
        "mov    %%rax, 5*8(%%rcx) \n"
        "mov    %%rax, 6*8(%%rcx) \n"
        "mov    %%rax, 7*8(%%rcx) \n"
        "mov    %%rax, 8*8(%%rcx) \n"
        "mov    %%rax, 9*8(%%rcx) \n"
        "mov    %%rax, 10*8(%%rcx) \n"
        "mov    %%rax, 11*8(%%rcx) \n"
        "mov    %%rax, 12*8(%%rcx) \n"
        "mov    %%rax, 13*8(%%rcx) \n"
        "mov    %%rax, 14*8(%%rcx) \n"
        "mov    %%rax, 15*8(%%rcx) \n"
        "lea    16*8(%%rcx), %%rcx \n"
        //"add    $16*8, %%rcx \n"
        // test write loop condition
        "cmp    %%rdi, %%rcx \n"        // compare to end iterator
        "jb     2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        :
        : [memarea] "g" (memarea), [size] "g" (size), [repeats] "r" (repeats)
        : "rax", "rcx", "rsi", "rdi");
}

REGISTER(ScanWrite64PtrUnrollLoop, 8, 8);

// 64-bit writer in a simple loop (C version)
void cScanRead64PtrSimpleLoop(void* memarea, size_t size, size_t repeats)
{
    uint64_t* begin = (uint64_t*)memarea;
    uint64_t* end = begin + size / sizeof(uint64_t);
    volatile uint64_t value;

    do {
        uint64_t* p = begin;
        do {
            value = *p++;
        }
        while (p < end);
    }
    while (--repeats != 0);
}

REGISTER(cScanRead64PtrSimpleLoop, 8, 8);

// 64-bit reader in a simple loop (Assembler version)
void ScanRead64PtrSimpleLoopAdd(void* memarea, size_t size, size_t repeats)
{
    asm("mov    %[memarea], %%rsi \n"   // rsi = memarea
        "mov    %[size], %%rdi \n"              
        "add    %%rsi, %%rdi \n"        // rdi = memarea+size
        "1: \n"                         // start of repeat loop
        "mov    %%rsi, %%rcx \n"        // rcx = reset loop iterator
        "2: \n"                         // start of write loop
        "mov    (%%rcx), %%rax \n"
        //"lea    8(%%rcx), %%rcx \n"
        "add    $8, %%rcx \n"
        // test write loop condition
        "cmp    %%rdi, %%rcx \n"        // compare to end iterator
        "jb     2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        :
        : [memarea] "g" (memarea), [size] "g" (size), [repeats] "r" (repeats)
        : "rax", "rcx", "rsi", "rdi");
}

REGISTER(ScanRead64PtrSimpleLoopAdd, 8, 8);

// 64-bit reader in a simple loop (Assembler version)
void ScanRead64PtrSimpleLoopLea(void* memarea, size_t size, size_t repeats)
{
    asm("mov    %[memarea], %%rsi \n"   // rsi = memarea
        "mov    %[size], %%rdi \n"              
        "add    %%rsi, %%rdi \n"        // rdi = memarea+size
        "1: \n"                         // start of repeat loop
        "mov    %%rsi, %%rcx \n"        // rcx = reset loop iterator
        "2: \n"                         // start of write loop
        "mov    (%%rcx), %%rax \n"
        "lea    8(%%rcx), %%rcx \n"
        // test write loop condition
        "cmp    %%rdi, %%rcx \n"        // compare to end iterator
        "jb     2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        :
        : [memarea] "g" (memarea), [size] "g" (size), [repeats] "r" (repeats)
        : "rax", "rcx", "rsi", "rdi");
}

REGISTER(ScanRead64PtrSimpleLoopLea, 8, 8);

// 64-bit writer in a simple loop (C version)
void cScanRead64PtrUnrollLoop(void* memarea, size_t size, size_t repeats)
{
    uint64_t* begin = (uint64_t*)memarea;
    uint64_t* end = begin + size / sizeof(uint64_t);
    volatile uint64_t value;

    do {
        uint64_t* p = begin;
        do {
            value = *(p+0);
            value = *(p+1);
            value = *(p+2);
            value = *(p+3);
            value = *(p+4);
            value = *(p+5);
            value = *(p+6);
            value = *(p+7);
            value = *(p+8);
            value = *(p+9);
            value = *(p+10);
            value = *(p+11);
            value = *(p+12);
            value = *(p+13);
            value = *(p+14);
            value = *(p+15);
            p += 16;
        }
        while (p < end);
    }
    while (--repeats != 0);
}

REGISTER(cScanRead64PtrUnrollLoop, 8, 8);

// 64-bit reader in an unrolled loop (Assembler version)
void ScanRead64PtrUnrollLoop(void* memarea, size_t size, size_t repeats)
{
    asm("mov    %[memarea], %%rsi \n"   // rsi = memarea
        "mov    %[size], %%rdi \n"              
        "add    %%rsi, %%rdi \n"        // rdi = memarea+size
        "1: \n"                         // start of repeat loop
        "mov    %%rsi, %%rcx \n"        // rcx = reset loop iterator
        "2: \n"                         // start of write loop
        "mov    0*8(%%rcx), %%rax \n"
        "mov    1*8(%%rcx), %%rax \n"
        "mov    2*8(%%rcx), %%rax \n"
        "mov    3*8(%%rcx), %%rax \n"
        "mov    4*8(%%rcx), %%rax \n"
        "mov    5*8(%%rcx), %%rax \n"
        "mov    6*8(%%rcx), %%rax \n"
        "mov    7*8(%%rcx), %%rax \n"
        "mov    8*8(%%rcx), %%rax \n"
        "mov    9*8(%%rcx), %%rax \n"
        "mov    10*8(%%rcx), %%rax \n"
        "mov    11*8(%%rcx), %%rax \n"
        "mov    12*8(%%rcx), %%rax \n"
        "mov    13*8(%%rcx), %%rax \n"
        "mov    14*8(%%rcx), %%rax \n"
        "mov    15*8(%%rcx), %%rax \n"
        "lea    16*8(%%rcx), %%rcx \n"
        //"add    $16*8, %%rcx \n"
        // test write loop condition
        "cmp    %%rdi, %%rcx \n"        // compare to end iterator
        "jb     2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        :
        : [memarea] "g" (memarea), [size] "g" (size), [repeats] "r" (repeats)
        : "rax", "rcx", "rsi", "rdi");
}

REGISTER(ScanRead64PtrUnrollLoop, 8, 8);

// -----------------------------------------------------------------------------

// 64-bit writer in an indexed loop (C version)
void cScanWrite64IndexSimpleLoop(void* _memarea, size_t _size, size_t repeats)
{
    uint64_t* memarea = (uint64_t*)_memarea;
    uint64_t size = _size / sizeof(uint64_t);
    uint64_t value = 0xC0FFEEEEBABE0000;

    do {
        for (size_t i = 0; i < size; ++i)
            memarea[i] = value;
    }
    while (--repeats != 0);
}

REGISTER(cScanWrite64IndexSimpleLoop, 8, 8);

// 64-bit writer in an indexed loop (Assembler version)
void ScanWrite64IndexSimpleLoop(void* memarea, size_t size, size_t repeats)
{
    asm("mov    $0xC0FFEEEEBABE0000, %%rax \n" // rax = test value
        "1: \n" // start of repeat loop
        "xor    %%rcx, %%rcx \n"        // rcx = reset index
        "2: \n" // start of write loop
        "mov    %%rax, (%[memarea],%%rcx) \n"
        //"lea    8(%%rcx), %%rcx \n"
        "add    $8, %%rcx \n"
        // test write loop condition
        "cmp    %[size], %%rcx \n"      // compare to total size
        "jb     2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        :
        : [memarea] "r" (memarea), [size] "r" (size), [repeats] "r" (repeats)
        : "rax", "rcx");
}

REGISTER(ScanWrite64IndexSimpleLoop, 8, 8);

// 64-bit writer in an indexed unrolled loop (C version)
void cScanWrite64IndexUnrollLoop(void* _memarea, size_t _size, size_t repeats)
{
    uint64_t* memarea = (uint64_t*)_memarea;
    uint64_t size = _size / sizeof(uint64_t);
    uint64_t value = 0xC0FFEEEEBABE0000;

    do {
        for (size_t i = 0; i < size; i += 16)
        {
            memarea[i+0] = value;
            memarea[i+1] = value;
            memarea[i+2] = value;
            memarea[i+3] = value;
            memarea[i+4] = value;
            memarea[i+5] = value;
            memarea[i+6] = value;
            memarea[i+7] = value;
            memarea[i+8] = value;
            memarea[i+9] = value;
            memarea[i+10] = value;
            memarea[i+11] = value;
            memarea[i+12] = value;
            memarea[i+13] = value;
            memarea[i+14] = value;
            memarea[i+15] = value;
        }
    }
    while (--repeats != 0);
}

REGISTER(cScanWrite64IndexUnrollLoop, 8, 8);

// 64-bit writer in an indexed unrolled loop (Assembler version)
void ScanWrite64IndexUnrollLoop(void* memarea, size_t size, size_t repeats)
{
    asm("mov    $0xC0FFEEEEBABE0000, %%rax \n" // rax = test value
        "1: \n" // start of repeat loop
        "xor    %%rcx, %%rcx \n"        // rcx = reset index
        "2: \n" // start of write loop
        "mov    %%rax, 0*8(%[memarea],%%rcx) \n"
        "mov    %%rax, 1*8(%[memarea],%%rcx) \n"
        "mov    %%rax, 2*8(%[memarea],%%rcx) \n"
        "mov    %%rax, 3*8(%[memarea],%%rcx) \n"
        "mov    %%rax, 4*8(%[memarea],%%rcx) \n"
        "mov    %%rax, 5*8(%[memarea],%%rcx) \n"
        "mov    %%rax, 6*8(%[memarea],%%rcx) \n"
        "mov    %%rax, 7*8(%[memarea],%%rcx) \n"
        "mov    %%rax, 8*8(%[memarea],%%rcx) \n"
        "mov    %%rax, 9*8(%[memarea],%%rcx) \n"
        "mov    %%rax, 10*8(%[memarea],%%rcx) \n"
        "mov    %%rax, 11*8(%[memarea],%%rcx) \n"
        "mov    %%rax, 12*8(%[memarea],%%rcx) \n"
        "mov    %%rax, 13*8(%[memarea],%%rcx) \n"
        "mov    %%rax, 14*8(%[memarea],%%rcx) \n"
        "mov    %%rax, 15*8(%[memarea],%%rcx) \n"
        //"lea    8(%%rcx), %%rcx \n"
        "add    $16*8, %%rcx \n"
        // test write loop condition
        "cmp    %[size], %%rcx \n"      // compare to total size
        "jb     2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        :
        : [memarea] "r" (memarea), [size] "r" (size), [repeats] "r" (repeats)
        : "rax", "rcx");
}

REGISTER(ScanWrite64IndexUnrollLoop, 8, 8);

// 64-bit reader in an indexed loop (Assembler version)
void ScanRead64IndexSimpleLoop(void* memarea, size_t size, size_t repeats)
{
    asm("1: \n" // start of repeat loop
        "xor    %%rcx, %%rcx \n"        // rcx = reset index
        "2: \n" // start of write loop
        "mov    (%[memarea],%%rcx), %%rax \n"
        //"lea    8(%%rcx), %%rcx \n"
        "add    $8, %%rcx \n"
        // test read loop condition
        "cmp    %[size], %%rcx \n"      // compare to total size
        "jb     2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        :
        : [memarea] "r" (memarea), [size] "r" (size), [repeats] "r" (repeats)
        : "rax", "rcx");
}

REGISTER(ScanRead64IndexSimpleLoop, 8, 8);

// 64-bit reader in an indexed unrolled loop (Assembler version)
void ScanRead64IndexUnrollLoop(void* memarea, size_t size, size_t repeats)
{
    asm("1: \n" // start of repeat loop
        "xor    %%rcx, %%rcx \n"        // rcx = reset index
        "2: \n" // start of write loop
        "mov    0*8(%[memarea],%%rcx), %%rax \n"
        "mov    1*8(%[memarea],%%rcx), %%rax \n"
        "mov    2*8(%[memarea],%%rcx), %%rax \n"
        "mov    3*8(%[memarea],%%rcx), %%rax \n"
        "mov    4*8(%[memarea],%%rcx), %%rax \n"
        "mov    5*8(%[memarea],%%rcx), %%rax \n"
        "mov    6*8(%[memarea],%%rcx), %%rax \n"
        "mov    7*8(%[memarea],%%rcx), %%rax \n"
        "mov    8*8(%[memarea],%%rcx), %%rax \n"
        "mov    9*8(%[memarea],%%rcx), %%rax \n"
        "mov    10*8(%[memarea],%%rcx), %%rax \n"
        "mov    11*8(%[memarea],%%rcx), %%rax \n"
        "mov    12*8(%[memarea],%%rcx), %%rax \n"
        "mov    13*8(%[memarea],%%rcx), %%rax \n"
        "mov    14*8(%[memarea],%%rcx), %%rax \n"
        "mov    15*8(%[memarea],%%rcx), %%rax \n"
        //"lea    8(%%rcx), %%rcx \n"
        "add    $16*8, %%rcx \n"
        // test read loop condition
        "cmp    %[size], %%rcx \n"      // compare to total size
        "jb     2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        :
        : [memarea] "r" (memarea), [size] "r" (size), [repeats] "r" (repeats)
        : "rax", "rcx");
}

REGISTER(ScanRead64IndexUnrollLoop, 8, 8);

// -----------------------------------------------------------------------------

#if 0
static const int skiplen64 = 64; // one item per cache line

// 64-bit skipping writer in a simple loop (C version)
void cSkipWrite64PtrSimpleLoop(void* memarea, size_t size, size_t repeats)
{
    uint64_t* begin = (uint64_t*)memarea;
    uint64_t* end = begin + size / sizeof(uint64_t);
    uint64_t value = 0xC0FFEEEEBABE0000;

    do {
        uint64_t* p = begin;
        do {
            *p++ = value;
            p += skiplen64 / sizeof(uint64_t);
        }
        while (p < end);
    }
    while (--repeats != 0);
}

REGISTER(cSkipWrite64PtrSimpleLoop, 8, 8);

// 64-bit skipping writer in a simple loop (Assembler version)
void SkipWrite64PtrSimpleLoop(void* memarea, size_t size, size_t repeats)
{
    asm("mov    $0xC0FFEEEEBABE0000, %%rax \n" // rax = test value
        "mov    %[memarea], %%rsi \n"   // rsi = memarea
        "mov    %[size], %%rdi \n"              
        "add    %%rsi, %%rdi \n"        // rdi = memarea+size
        "1: \n" // start of repeat loop
        "mov    %%rsi, %%rcx \n"        // rcx = reset loop iterator
        "2: \n" // start of write loop
        "mov    %%rax, (%%rcx) \n"
        //"lea    8(%%rcx), %%rcx \n"
        "add    $8, %%rcx \n"
        // test write loop condition
        "cmp    %%rdi, %%rcx \n"        // compare to end iterator
        "jb     2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        :
        : [memarea] "g" (memarea), [size] "g" (size), [repeats] "r" (repeats)
        : "rax", "rcx", "rsi", "rdi");
}

REGISTER(SkipWrite64PtrSimpleLoop, 8, 8);
#endif

// -----------------------------------------------------------------------------

// follow 64-bit permutation in a simple loop (C version)
void cPermRead64SimpleLoop(void* memarea, size_t, size_t repeats)
{
    uint64_t* begin = (uint64_t*)memarea;

    do {
        uint64_t* p = begin;
        do {
            p = (uint64_t*)*p;
        }
        while (p != begin);
    }
    while (--repeats != 0);
}

REGISTER_PERM(cPermRead64SimpleLoop, 8);

// follow 64-bit permutation in a simple loop (Assembler version)
void PermRead64SimpleLoop(void* memarea, size_t, size_t repeats)
{
    asm("1: \n" // start of repeat loop
        "mov    %[memarea], %%rax \n"   // rax = reset iterator
        "2: \n" // start of write loop
        "mov    (%%rax), %%rax \n"
        // test write loop condition
        "cmp    %%rax, %[memarea] \n"   // compare to first iterator
        "jne    2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        :
        : [memarea] "r" (memarea), [repeats] "r" (repeats)
        : "rax");
}

REGISTER_PERM(PermRead64SimpleLoop, 8);

// follow 64-bit permutation in an unrolled loop (C version)
void cPermRead64UnrollLoop(void* memarea, size_t, size_t repeats)
{
    uint64_t* begin = (uint64_t*)memarea;

    do {
        uint64_t* p = begin;
        do {
            p = (uint64_t*)*p;
            p = (uint64_t*)*p;
            p = (uint64_t*)*p;
            p = (uint64_t*)*p;

            p = (uint64_t*)*p;
            p = (uint64_t*)*p;
            p = (uint64_t*)*p;
            p = (uint64_t*)*p;

            p = (uint64_t*)*p;
            p = (uint64_t*)*p;
            p = (uint64_t*)*p;
            p = (uint64_t*)*p;

            p = (uint64_t*)*p;
            p = (uint64_t*)*p;
            p = (uint64_t*)*p;
            p = (uint64_t*)*p;
        }
        while (p != begin);
    }
    while (--repeats != 0);
}

REGISTER_PERM(cPermRead64UnrollLoop, 8);

// follow 64-bit permutation in an unrolled loop (Assembler version)
void PermRead64UnrollLoop(void* memarea, size_t, size_t repeats)
{
    asm("1: \n" // start of repeat loop
        "mov    %[memarea], %%rax \n"   // rax = reset iterator
        "2: \n" // start of write loop
        "mov    (%%rax), %%rax \n"
        "mov    (%%rax), %%rax \n"
        "mov    (%%rax), %%rax \n"
        "mov    (%%rax), %%rax \n"

        "mov    (%%rax), %%rax \n"
        "mov    (%%rax), %%rax \n"
        "mov    (%%rax), %%rax \n"
        "mov    (%%rax), %%rax \n"

        "mov    (%%rax), %%rax \n"
        "mov    (%%rax), %%rax \n"
        "mov    (%%rax), %%rax \n"
        "mov    (%%rax), %%rax \n"

        "mov    (%%rax), %%rax \n"
        "mov    (%%rax), %%rax \n"
        "mov    (%%rax), %%rax \n"
        "mov    (%%rax), %%rax \n"
        // test write loop condition
        "cmp    %%rax, %[memarea] \n"   // compare to first iterator
        "jne    2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        :
        : [memarea] "r" (memarea), [repeats] "r" (repeats)
        : "rax");
}

REGISTER_PERM(PermRead64UnrollLoop, 8);

// -----------------------------------------------------------------------------
