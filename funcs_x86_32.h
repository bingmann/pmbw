/******************************************************************************
 * funcs_x86_32.h
 *
 * All Test Functions in 32-bit x86 assembly code: they are codenamed as
 * Scan/Perm Read/Write 32/64 Ptr/Index Simple/Unroll Loop.
 *
 * Scan = consecutive scanning, Perm = walk permutation cycle.
 * Read/Write = obvious
 * 32/64 = size of access
 * Ptr = with pointer, Index = access as array[i]
 * Simple/Unroll = 1 or 16 operations per loop
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

// ****************************************************************************
// ----------------------------------------------------------------------------
// 32-bit Operations
// ----------------------------------------------------------------------------
// ****************************************************************************

// 32-bit writer in a simple loop (C version)
void cScanWrite32PtrSimpleLoop(char* memarea, size_t size, size_t repeats)
{
    uint32_t* begin = (uint32_t*)memarea;
    uint32_t* end = begin + size / sizeof(uint32_t);
    uint32_t value = 0xC0FFEEEE;

    do {
        uint32_t* p = begin;
        do {
            *p++ = value;
        }
        while (p < end);
    }
    while (--repeats != 0);
}

//REGISTER(cScanWrite32PtrSimpleLoop, 4, 4);

// 32-bit writer in a simple loop (Assembler version)
void ScanWrite32PtrSimpleLoop(char* memarea, size_t size, size_t repeats)
{
    asm("mov    $0xC0FFEEEE, %%eax \n"  // rax = test value
        "1: \n" // start of repeat loop
        "mov    %[memarea], %%ecx \n"   // rcx = reset loop iterator
        "2: \n" // start of write loop
        "mov    %%eax, (%%ecx) \n"
        "add    $4, %%ecx \n"
        // test write loop condition
        "cmp    %[end], %%ecx \n"       // compare to end iterator
        "jb     2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        :
        : [memarea] "r" (memarea), [end] "r" (memarea+size), [repeats] "r" (repeats)
        : "eax", "ecx");
}

REGISTER(ScanWrite32PtrSimpleLoop, 4, 4);

// 32-bit writer in an unrolled loop (Assembler version)
void ScanWrite32PtrUnrollLoop(char* memarea, size_t size, size_t repeats)
{
    asm("mov    $0xC0FFEEEE, %%eax \n"  // rax = test value
        "1: \n" // start of repeat loop
        "mov    %[memarea], %%ecx \n"   // rcx = reset loop iterator
        "2: \n" // start of write loop
        "mov    %%eax, 0*4(%%ecx) \n"
        "mov    %%eax, 1*4(%%ecx) \n"
        "mov    %%eax, 2*4(%%ecx) \n"
        "mov    %%eax, 3*4(%%ecx) \n"
        "mov    %%eax, 4*4(%%ecx) \n"
        "mov    %%eax, 5*4(%%ecx) \n"
        "mov    %%eax, 6*4(%%ecx) \n"
        "mov    %%eax, 7*4(%%ecx) \n"
        "mov    %%eax, 8*4(%%ecx) \n"
        "mov    %%eax, 9*4(%%ecx) \n"
        "mov    %%eax, 10*4(%%ecx) \n"
        "mov    %%eax, 11*4(%%ecx) \n"
        "mov    %%eax, 12*4(%%ecx) \n"
        "mov    %%eax, 13*4(%%ecx) \n"
        "mov    %%eax, 14*4(%%ecx) \n"
        "mov    %%eax, 15*4(%%ecx) \n"
        "add    $16*4, %%ecx \n"
        // test write loop condition
        "cmp    %[end], %%ecx \n"       // compare to end iterator
        "jb     2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        :
        : [memarea] "r" (memarea), [end] "r" (memarea+size), [repeats] "r" (repeats)
        : "eax", "ecx");
}

REGISTER(ScanWrite32PtrUnrollLoop, 4, 4);

// 32-bit reader in a simple loop (Assembler version)
void ScanRead32PtrSimpleLoop(char* memarea, size_t size, size_t repeats)
{
    asm("1: \n" // start of repeat loop
        "mov    %[memarea], %%ecx \n"   // rcx = reset loop iterator
        "2: \n" // start of read loop
        "mov    (%%ecx), %%eax \n"
        "add    $4, %%ecx \n"
        // test read loop condition
        "cmp    %[end], %%ecx \n"       // compare to end iterator
        "jb     2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        :
        : [memarea] "r" (memarea), [end] "r" (memarea+size), [repeats] "r" (repeats)
        : "eax", "ecx");
}

REGISTER(ScanRead32PtrSimpleLoop, 4, 4);

// 32-bit reader in an unrolled loop (Assembler version)
void ScanRead32PtrUnrollLoop(char* memarea, size_t size, size_t repeats)
{
    asm("1: \n" // start of repeat loop
        "mov    %[memarea], %%ecx \n"   // rcx = reset loop iterator
        "2: \n" // start of read loop
        "mov    0*4(%%ecx), %%eax \n"
        "mov    1*4(%%ecx), %%eax \n"
        "mov    2*4(%%ecx), %%eax \n"
        "mov    3*4(%%ecx), %%eax \n"
        "mov    4*4(%%ecx), %%eax \n"
        "mov    5*4(%%ecx), %%eax \n"
        "mov    6*4(%%ecx), %%eax \n"
        "mov    7*4(%%ecx), %%eax \n"
        "mov    8*4(%%ecx), %%eax \n"
        "mov    9*4(%%ecx), %%eax \n"
        "mov    10*4(%%ecx), %%eax \n"
        "mov    11*4(%%ecx), %%eax \n"
        "mov    12*4(%%ecx), %%eax \n"
        "mov    13*4(%%ecx), %%eax \n"
        "mov    14*4(%%ecx), %%eax \n"
        "mov    15*4(%%ecx), %%eax \n"
        "add    $16*4, %%ecx \n"
        // test read loop condition
        "cmp    %[end], %%ecx \n"       // compare to end iterator
        "jb     2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        :
        : [memarea] "r" (memarea), [end] "r" (memarea+size), [repeats] "r" (repeats)
        : "eax", "ecx");
}

REGISTER(ScanRead32PtrUnrollLoop, 4, 4);

// -----------------------------------------------------------------------------

// 32-bit writer in an indexed loop (C version)
void cScanWrite32IndexSimpleLoop(char* _memarea, size_t _size, size_t repeats)
{
    uint32_t* memarea = (uint32_t*)_memarea;
    uint32_t size = _size / sizeof(uint32_t);
    uint32_t value = 0xC0FFEEEE;

    do {
        for (size_t i = 0; i < size; ++i)
            memarea[i] = value;
    }
    while (--repeats != 0);
}

//REGISTER(cScanWrite32IndexSimpleLoop, 4, 4);

// 32-bit writer in an indexed loop (Assembler version)
void ScanWrite32IndexSimpleLoop(char* memarea, size_t size, size_t repeats)
{
    asm("mov    $0xC0FFEEEE, %%eax \n" // rax = test value
        "1: \n" // start of repeat loop
        "xor    %%ecx, %%ecx \n"        // rcx = reset index
        "2: \n" // start of write loop
        "mov    %%eax, (%[memarea],%%ecx) \n"
        "add    $4, %%ecx \n"
        // test write loop condition
        "cmp    %[size], %%ecx \n"      // compare to total size
        "jb     2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        :
        : [memarea] "r" (memarea), [size] "r" (size), [repeats] "r" (repeats)
        : "eax", "ecx");
}

REGISTER(ScanWrite32IndexSimpleLoop, 4, 4);

// 32-bit writer in an indexed unrolled loop (Assembler version)
void ScanWrite32IndexUnrollLoop(char* memarea, size_t size, size_t repeats)
{
    asm("mov    $0xC0FFEEEE, %%eax \n" // rax = test value
        "1: \n" // start of repeat loop
        "xor    %%ecx, %%ecx \n"        // rcx = reset index
        "2: \n" // start of write loop
        "mov    %%eax, 0*4(%[memarea],%%ecx) \n"
        "mov    %%eax, 1*4(%[memarea],%%ecx) \n"
        "mov    %%eax, 2*4(%[memarea],%%ecx) \n"
        "mov    %%eax, 3*4(%[memarea],%%ecx) \n"
        "mov    %%eax, 4*4(%[memarea],%%ecx) \n"
        "mov    %%eax, 5*4(%[memarea],%%ecx) \n"
        "mov    %%eax, 6*4(%[memarea],%%ecx) \n"
        "mov    %%eax, 7*4(%[memarea],%%ecx) \n"
        "mov    %%eax, 8*4(%[memarea],%%ecx) \n"
        "mov    %%eax, 9*4(%[memarea],%%ecx) \n"
        "mov    %%eax, 10*4(%[memarea],%%ecx) \n"
        "mov    %%eax, 11*4(%[memarea],%%ecx) \n"
        "mov    %%eax, 12*4(%[memarea],%%ecx) \n"
        "mov    %%eax, 13*4(%[memarea],%%ecx) \n"
        "mov    %%eax, 14*4(%[memarea],%%ecx) \n"
        "mov    %%eax, 15*4(%[memarea],%%ecx) \n"
        "add    $16*4, %%ecx \n"
        // test write loop condition
        "cmp    %[size], %%ecx \n"      // compare to total size
        "jb     2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        :
        : [memarea] "r" (memarea), [size] "r" (size), [repeats] "r" (repeats)
        : "eax", "ecx");
}

REGISTER(ScanWrite32IndexUnrollLoop, 4, 4);

// 32-bit reader in an indexed loop (Assembler version)
void ScanRead32IndexSimpleLoop(char* memarea, size_t size, size_t repeats)
{
    asm("1: \n" // start of repeat loop
        "xor    %%ecx, %%ecx \n"        // rcx = reset index
        "2: \n" // start of read loop
        "mov    (%[memarea],%%ecx), %%eax \n"
        "add    $4, %%ecx \n"
        // test read loop condition
        "cmp    %[size], %%ecx \n"      // compare to total size
        "jb     2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        :
        : [memarea] "r" (memarea), [size] "r" (size), [repeats] "r" (repeats)
        : "eax", "ecx");
}

REGISTER(ScanRead32IndexSimpleLoop, 4, 4);

// 32-bit reader in an indexed unrolled loop (Assembler version)
void ScanRead32IndexUnrollLoop(char* memarea, size_t size, size_t repeats)
{
    asm("1: \n" // start of repeat loop
        "xor    %%ecx, %%ecx \n"        // rcx = reset index
        "2: \n" // start of read loop
        "mov    0*4(%[memarea],%%ecx), %%eax \n"
        "mov    1*4(%[memarea],%%ecx), %%eax \n"
        "mov    2*4(%[memarea],%%ecx), %%eax \n"
        "mov    3*4(%[memarea],%%ecx), %%eax \n"
        "mov    4*4(%[memarea],%%ecx), %%eax \n"
        "mov    5*4(%[memarea],%%ecx), %%eax \n"
        "mov    6*4(%[memarea],%%ecx), %%eax \n"
        "mov    7*4(%[memarea],%%ecx), %%eax \n"
        "mov    8*4(%[memarea],%%ecx), %%eax \n"
        "mov    9*4(%[memarea],%%ecx), %%eax \n"
        "mov    10*4(%[memarea],%%ecx), %%eax \n"
        "mov    11*4(%[memarea],%%ecx), %%eax \n"
        "mov    12*4(%[memarea],%%ecx), %%eax \n"
        "mov    13*4(%[memarea],%%ecx), %%eax \n"
        "mov    14*4(%[memarea],%%ecx), %%eax \n"
        "mov    15*4(%[memarea],%%ecx), %%eax \n"
        "add    $16*4, %%ecx \n"
        // test read loop condition
        "cmp    %[size], %%ecx \n"      // compare to total size
        "jb     2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        :
        : [memarea] "r" (memarea), [size] "r" (size), [repeats] "r" (repeats)
        : "eax", "ecx");
}

REGISTER(ScanRead32IndexUnrollLoop, 4, 4);

// ****************************************************************************
// ----------------------------------------------------------------------------
// 64-bit Operations
// ----------------------------------------------------------------------------
// ****************************************************************************

// 64-bit writer in a simple loop (Assembler version)
void ScanWrite64PtrSimpleLoop(char* memarea, size_t size, size_t repeats)
{
    char fxsave[512] __attribute__((aligned(16)));

    asm("fxsave %[fxsave] \n"           // save FPU state
        "mov    $0xC0FFEEEE, %%eax \n"
        "movd   %%eax, %%mm0 \n"
        "punpckldq %%mm0, %%mm0 \n"
        "1: \n" // start of repeat loop
        "mov    %[memarea], %%eax \n"   // rax = reset loop iterator
        "2: \n" // start of write loop
        "movq   %%mm0, (%%eax) \n"
        "add    $8, %%eax \n"
        // test write loop condition
        "cmp    %[end], %%eax \n"       // compare to end iterator
        "jb     2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        "fxrstor %[fxsave] \n"          // restore FPU state
        :
        : [memarea] "r" (memarea), [end] "r" (memarea+size), [repeats] "r" (repeats),
          [fxsave] "m" (*fxsave)
        : "eax");
}

REGISTER_CPUFEAT(ScanWrite64PtrSimpleLoop, "mmx", 8, 8);

// 64-bit writer in an unrolled loop (Assembler version)
void ScanWrite64PtrUnrollLoop(char* memarea, size_t size, size_t repeats)
{
    char fxsave[512] __attribute__((aligned(16)));

    asm("fxsave %[fxsave] \n"           // save FPU state
        "mov    $0xC0FFEEEE, %%eax \n"
        "movd   %%eax, %%mm0 \n"
        "punpckldq %%mm0, %%mm0 \n"
        "1: \n" // start of repeat loop
        "mov    %[memarea], %%eax \n"   // rax = reset loop iterator
        "2: \n" // start of write loop
        "movq   %%mm0, 0*16(%%eax) \n"
        "movq   %%mm0, 1*16(%%eax) \n"
        "movq   %%mm0, 2*16(%%eax) \n"
        "movq   %%mm0, 3*16(%%eax) \n"

        "movq   %%mm0, 4*16(%%eax) \n"
        "movq   %%mm0, 5*16(%%eax) \n"
        "movq   %%mm0, 6*16(%%eax) \n"
        "movq   %%mm0, 7*16(%%eax) \n"

        "movq   %%mm0, 8*16(%%eax) \n"
        "movq   %%mm0, 9*16(%%eax) \n"
        "movq   %%mm0, 10*16(%%eax) \n"
        "movq   %%mm0, 11*16(%%eax) \n"

        "movq   %%mm0, 12*16(%%eax) \n"
        "movq   %%mm0, 13*16(%%eax) \n"
        "movq   %%mm0, 14*16(%%eax) \n"
        "movq   %%mm0, 15*16(%%eax) \n"

        "add    $16*8, %%eax \n"
        // test write loop condition
        "cmp    %[end], %%eax \n"       // compare to end iterator
        "jb     2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        "fxrstor %[fxsave] \n"          // restore FPU state
        :
        : [memarea] "r" (memarea), [end] "r" (memarea+size), [repeats] "r" (repeats),
          [fxsave] "m" (*fxsave)
        : "eax");
}

REGISTER_CPUFEAT(ScanWrite64PtrUnrollLoop, "mmx", 8, 8);

// 64-bit reader in a simple loop (Assembler version)
void ScanRead64PtrSimpleLoop(char* memarea, size_t size, size_t repeats)
{
    char fxsave[512] __attribute__((aligned(16)));

    asm("fxsave %[fxsave] \n"           // save FPU state
        "1: \n" // start of repeat loop
        "mov    %[memarea], %%eax \n"   // rax = reset loop iterator
        "2: \n" // start of read loop
        "movq   (%%eax), %%mm0 \n"
        "add    $8, %%eax \n"
        // test read loop condition
        "cmp    %[end], %%eax \n"       // compare to end iterator
        "jb     2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        "fxrstor %[fxsave] \n"          // restore FPU state
        :
        : [memarea] "r" (memarea), [end] "r" (memarea+size), [repeats] "r" (repeats),
          [fxsave] "m" (*fxsave)
        : "eax");
}

REGISTER_CPUFEAT(ScanRead64PtrSimpleLoop, "mmx", 8, 8);

// 64-bit reader in an unrolled loop (Assembler version)
void ScanRead64PtrUnrollLoop(char* memarea, size_t size, size_t repeats)
{
    char fxsave[512] __attribute__((aligned(16)));

    asm("fxsave %[fxsave] \n"           // save FPU state
        "1: \n" // start of repeat loop
        "mov    %[memarea], %%eax \n"   // rax = reset loop iterator
        "2: \n" // start of read loop
        "movq   0*8(%%eax), %%mm0 \n"
        "movq   1*8(%%eax), %%mm0 \n"
        "movq   2*8(%%eax), %%mm0 \n"
        "movq   3*8(%%eax), %%mm0 \n"

        "movq   4*8(%%eax), %%mm0 \n"
        "movq   5*8(%%eax), %%mm0 \n"
        "movq   6*8(%%eax), %%mm0 \n"
        "movq   7*8(%%eax), %%mm0 \n"

        "movq   8*8(%%eax), %%mm0 \n"
        "movq   9*8(%%eax), %%mm0 \n"
        "movq   10*8(%%eax), %%mm0 \n"
        "movq   11*8(%%eax), %%mm0 \n"

        "movq   12*8(%%eax), %%mm0 \n"
        "movq   13*8(%%eax), %%mm0 \n"
        "movq   14*8(%%eax), %%mm0 \n"
        "movq   15*8(%%eax), %%mm0 \n"

        "add    $16*8, %%eax \n"
        // test read loop condition
        "cmp    %[end], %%eax \n"       // compare to end iterator
        "jb     2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        "fxrstor %[fxsave] \n"          // restore FPU state
        :
        : [memarea] "r" (memarea), [end] "r" (memarea+size), [repeats] "r" (repeats),
          [fxsave] "m" (*fxsave)
        : "eax");
}

REGISTER_CPUFEAT(ScanRead64PtrUnrollLoop, "mmx", 8, 8);

// ****************************************************************************
// ----------------------------------------------------------------------------
// 128-bit Operations
// ----------------------------------------------------------------------------
// ****************************************************************************

// 128-bit writer in a simple loop (Assembler version)
void ScanWrite128PtrSimpleLoop(char* memarea, size_t size, size_t repeats)
{
    char fxsave[512] __attribute__((aligned(16)));

    asm("fxsave %[fxsave] \n"           // save FPU state
        "mov    $0xC0FFEEEE, %%eax \n"
        "movd   %%eax, %%xmm0 \n"
        "punpckldq %%xmm0, %%xmm0 \n"
        "movlhps %%xmm0, %%xmm0 \n"     // xmm0 = test value
        "1: \n" // start of repeat loop
        "mov    %[memarea], %%eax \n"   // rax = reset loop iterator
        "2: \n" // start of write loop
        "movdqa %%xmm0, (%%eax) \n"
        "add    $16, %%eax \n"
        // test write loop condition
        "cmp    %[end], %%eax \n"       // compare to end iterator
        "jb     2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        "fxrstor %[fxsave] \n"          // restore FPU state
        :
        : [memarea] "r" (memarea), [end] "r" (memarea+size), [repeats] "r" (repeats),
          [fxsave] "m" (*fxsave)
        : "eax");
}

REGISTER_CPUFEAT(ScanWrite128PtrSimpleLoop, "sse", 16, 16);

// 128-bit writer in an unrolled loop (Assembler version)
void ScanWrite128PtrUnrollLoop(char* memarea, size_t size, size_t repeats)
{
    char fxsave[512] __attribute__((aligned(16)));

    asm("fxsave %[fxsave] \n"           // save FPU state
        "mov    $0xC0FFEEEE, %%eax \n"
        "movd   %%eax, %%xmm0 \n"
        "punpckldq %%xmm0, %%xmm0 \n"
        "movlhps %%xmm0, %%xmm0 \n"     // xmm0 = test value
        "1: \n" // start of repeat loop
        "mov    %[memarea], %%eax \n"   // rax = reset loop iterator
        "2: \n" // start of write loop
        "movdqa %%xmm0, 0*16(%%eax) \n"
        "movdqa %%xmm0, 1*16(%%eax) \n"
        "movdqa %%xmm0, 2*16(%%eax) \n"
        "movdqa %%xmm0, 3*16(%%eax) \n"

        "movdqa %%xmm0, 4*16(%%eax) \n"
        "movdqa %%xmm0, 5*16(%%eax) \n"
        "movdqa %%xmm0, 6*16(%%eax) \n"
        "movdqa %%xmm0, 7*16(%%eax) \n"

        "movdqa %%xmm0, 8*16(%%eax) \n"
        "movdqa %%xmm0, 9*16(%%eax) \n"
        "movdqa %%xmm0, 10*16(%%eax) \n"
        "movdqa %%xmm0, 11*16(%%eax) \n"

        "movdqa %%xmm0, 12*16(%%eax) \n"
        "movdqa %%xmm0, 13*16(%%eax) \n"
        "movdqa %%xmm0, 14*16(%%eax) \n"
        "movdqa %%xmm0, 15*16(%%eax) \n"

        "add    $16*16, %%eax \n"
        // test write loop condition
        "cmp    %[end], %%eax \n"       // compare to end iterator
        "jb     2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        "fxrstor %[fxsave] \n"          // restore FPU state
        :
        : [memarea] "r" (memarea), [end] "r" (memarea+size), [repeats] "r" (repeats),
          [fxsave] "m" (*fxsave)
        : "eax");
}

REGISTER_CPUFEAT(ScanWrite128PtrUnrollLoop, "sse", 16, 16);

// 128-bit reader in a simple loop (Assembler version)
void ScanRead128PtrSimpleLoop(char* memarea, size_t size, size_t repeats)
{
    char fxsave[512] __attribute__((aligned(16)));

    asm("fxsave %[fxsave] \n"           // save FPU state
        "1: \n" // start of repeat loop
        "mov    %[memarea], %%eax \n"   // rax = reset loop iterator
        "2: \n" // start of read loop
        "movdqa (%%eax), %%xmm0 \n"
        "add    $16, %%eax \n"
        // test read loop condition
        "cmp    %[end], %%eax \n"       // compare to end iterator
        "jb     2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        "fxrstor %[fxsave] \n"          // restore FPU state
        :
        : [memarea] "r" (memarea), [end] "r" (memarea+size), [repeats] "r" (repeats),
          [fxsave] "m" (*fxsave)
        : "eax");
}

REGISTER_CPUFEAT(ScanRead128PtrSimpleLoop, "sse", 8, 8);

// 128-bit reader in an unrolled loop (Assembler version)
void ScanRead128PtrUnrollLoop(char* memarea, size_t size, size_t repeats)
{
    char fxsave[512] __attribute__((aligned(16)));

    asm("fxsave %[fxsave] \n"           // save FPU state
        "1: \n" // start of repeat loop
        "mov    %[memarea], %%eax \n"   // rax = reset loop iterator
        "2: \n" // start of read loop
        "movdqa 0*16(%%eax), %%xmm0 \n"
        "movdqa 1*16(%%eax), %%xmm0 \n"
        "movdqa 2*16(%%eax), %%xmm0 \n"
        "movdqa 3*16(%%eax), %%xmm0 \n"

        "movdqa 4*16(%%eax), %%xmm0 \n"
        "movdqa 5*16(%%eax), %%xmm0 \n"
        "movdqa 6*16(%%eax), %%xmm0 \n"
        "movdqa 7*16(%%eax), %%xmm0 \n"

        "movdqa 8*16(%%eax), %%xmm0 \n"
        "movdqa 9*16(%%eax), %%xmm0 \n"
        "movdqa 10*16(%%eax), %%xmm0 \n"
        "movdqa 11*16(%%eax), %%xmm0 \n"

        "movdqa 12*16(%%eax), %%xmm0 \n"
        "movdqa 13*16(%%eax), %%xmm0 \n"
        "movdqa 14*16(%%eax), %%xmm0 \n"
        "movdqa 15*16(%%eax), %%xmm0 \n"

        "add    $16*16, %%eax \n"
        // test read loop condition
        "cmp    %[end], %%eax \n"       // compare to end iterator
        "jb     2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        "fxrstor %[fxsave] \n"          // restore FPU state
        :
        : [memarea] "r" (memarea), [end] "r" (memarea+size), [repeats] "r" (repeats),
          [fxsave] "m" (*fxsave)
        : "eax");
}

REGISTER_CPUFEAT(ScanRead128PtrUnrollLoop, "sse", 8, 8);

// ****************************************************************************
// ----------------------------------------------------------------------------
// 16-bit Operations
// ----------------------------------------------------------------------------
// ****************************************************************************

// 16-bit writer in a simple loop (Assembler version)
void ScanWrite16PtrSimpleLoop(char* memarea, size_t size, size_t repeats)
{
    asm("mov    $0xCFFE, %%ax \n"       // ax = test value
        "1: \n" // start of repeat loop
        "mov    %[memarea], %%ecx \n"   // rcx = reset loop iterator
        "2: \n" // start of write loop
        "mov    %%ax, (%%ecx) \n"
        "add    $2, %%ecx \n"
        // test write loop condition
        "cmp    %[end], %%ecx \n"       // compare to end iterator
        "jb     2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        :
        : [memarea] "r" (memarea), [end] "r" (memarea+size), [repeats] "r" (repeats)
        : "eax", "ecx");
}

REGISTER(ScanWrite16PtrSimpleLoop, 2, 2);

// 16-bit writer in an unrolled loop (Assembler version)
void ScanWrite16PtrUnrollLoop(char* memarea, size_t size, size_t repeats)
{
    asm("mov    $0xC0FFEEEE, %%eax \n"  // eax = test value
        "1: \n" // start of repeat loop
        "mov    %[memarea], %%ecx \n"   // rcx = reset loop iterator
        "2: \n" // start of write loop
        "mov    %%ax, 0*2(%%ecx) \n"
        "mov    %%ax, 1*2(%%ecx) \n"
        "mov    %%ax, 2*2(%%ecx) \n"
        "mov    %%ax, 3*2(%%ecx) \n"
        "mov    %%ax, 4*2(%%ecx) \n"
        "mov    %%ax, 5*2(%%ecx) \n"
        "mov    %%ax, 6*2(%%ecx) \n"
        "mov    %%ax, 7*2(%%ecx) \n"
        "mov    %%ax, 8*2(%%ecx) \n"
        "mov    %%ax, 9*2(%%ecx) \n"
        "mov    %%ax, 10*2(%%ecx) \n"
        "mov    %%ax, 11*2(%%ecx) \n"
        "mov    %%ax, 12*2(%%ecx) \n"
        "mov    %%ax, 13*2(%%ecx) \n"
        "mov    %%ax, 14*2(%%ecx) \n"
        "mov    %%ax, 15*2(%%ecx) \n"
        "add    $16*2, %%ecx \n"
        // test write loop condition
        "cmp    %[end], %%ecx \n"       // compare to end iterator
        "jb     2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        :
        : [memarea] "r" (memarea), [end] "r" (memarea+size), [repeats] "r" (repeats)
        : "eax", "ecx");
}

REGISTER(ScanWrite16PtrUnrollLoop, 2, 2);

// 16-bit reader in a simple loop (Assembler version)
void ScanRead16PtrSimpleLoop(char* memarea, size_t size, size_t repeats)
{
    asm("1: \n" // start of repeat loop
        "mov    %[memarea], %%ecx \n"   // rcx = reset loop iterator
        "2: \n" // start of read loop
        "mov    (%%ecx), %%eax \n"
        "add    $2, %%ecx \n"
        // test read loop condition
        "cmp    %[end], %%ecx \n"       // compare to end iterator
        "jb     2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        :
        : [memarea] "r" (memarea), [end] "r" (memarea+size), [repeats] "r" (repeats)
        : "eax", "ecx");
}

REGISTER(ScanRead16PtrSimpleLoop, 2, 2);

// 16-bit reader in an unrolled loop (Assembler version)
void ScanRead16PtrUnrollLoop(char* memarea, size_t size, size_t repeats)
{
    asm("1: \n" // start of repeat loop
        "mov    %[memarea], %%ecx \n"   // rcx = reset loop iterator
        "2: \n" // start of read loop
        "mov    0*2(%%ecx), %%eax \n"
        "mov    1*2(%%ecx), %%eax \n"
        "mov    2*2(%%ecx), %%eax \n"
        "mov    3*2(%%ecx), %%eax \n"
        "mov    4*2(%%ecx), %%eax \n"
        "mov    5*2(%%ecx), %%eax \n"
        "mov    6*2(%%ecx), %%eax \n"
        "mov    7*2(%%ecx), %%eax \n"
        "mov    8*2(%%ecx), %%eax \n"
        "mov    9*2(%%ecx), %%eax \n"
        "mov    10*2(%%ecx), %%eax \n"
        "mov    11*2(%%ecx), %%eax \n"
        "mov    12*2(%%ecx), %%eax \n"
        "mov    13*2(%%ecx), %%eax \n"
        "mov    14*2(%%ecx), %%eax \n"
        "mov    15*2(%%ecx), %%eax \n"
        "add    $16*2, %%ecx \n"
        // test read loop condition
        "cmp    %[end], %%ecx \n"       // compare to end iterator
        "jb     2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        :
        : [memarea] "r" (memarea), [end] "r" (memarea+size), [repeats] "r" (repeats)
        : "eax", "ecx");
}

REGISTER(ScanRead16PtrUnrollLoop, 2, 2);

// ****************************************************************************
// ----------------------------------------------------------------------------
// Permutation Walking
// ----------------------------------------------------------------------------
// ****************************************************************************

// follow 32-bit permutation in a simple loop (C version)
void cPermRead32SimpleLoop(char* memarea, size_t, size_t repeats)
{
    uint32_t* begin = (uint32_t*)memarea;

    do {
        uint32_t* p = begin;
        do {
            p = (uint32_t*)*p;
        }
        while (p != begin);
    }
    while (--repeats != 0);
}

//REGISTER_PERM(cPermRead32SimpleLoop, 8);

// follow 32-bit permutation in a simple loop (Assembler version)
void PermRead32SimpleLoop(char* memarea, size_t, size_t repeats)
{
    asm("1: \n" // start of repeat loop
        "mov    %[memarea], %%eax \n"   // rax = reset iterator
        "2: \n" // start of read loop
        "mov    (%%eax), %%eax \n"
        // test read loop condition
        "cmp    %%eax, %[memarea] \n"   // compare to first iterator
        "jne    2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        :
        : [memarea] "r" (memarea), [repeats] "r" (repeats)
        : "eax");
}

REGISTER_PERM(PermRead32SimpleLoop, 8);

// follow 32-bit permutation in an unrolled loop (Assembler version)
void PermRead32UnrollLoop(char* memarea, size_t, size_t repeats)
{
    asm("1: \n" // start of repeat loop
        "mov    %[memarea], %%eax \n"   // rax = reset iterator
        "2: \n" // start of read loop
        "mov    (%%eax), %%eax \n"
        "mov    (%%eax), %%eax \n"
        "mov    (%%eax), %%eax \n"
        "mov    (%%eax), %%eax \n"

        "mov    (%%eax), %%eax \n"
        "mov    (%%eax), %%eax \n"
        "mov    (%%eax), %%eax \n"
        "mov    (%%eax), %%eax \n"

        "mov    (%%eax), %%eax \n"
        "mov    (%%eax), %%eax \n"
        "mov    (%%eax), %%eax \n"
        "mov    (%%eax), %%eax \n"

        "mov    (%%eax), %%eax \n"
        "mov    (%%eax), %%eax \n"
        "mov    (%%eax), %%eax \n"
        "mov    (%%eax), %%eax \n"
        // test read loop condition
        "cmp    %%eax, %[memarea] \n"   // compare to first iterator
        "jne    2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        :
        : [memarea] "r" (memarea), [repeats] "r" (repeats)
        : "eax");
}

REGISTER_PERM(PermRead32UnrollLoop, 8);

// -----------------------------------------------------------------------------
