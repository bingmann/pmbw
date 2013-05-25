/******************************************************************************
 * funcs_x86_128.h
 *
 * All 128-bit Test Functions: they are codenamed as
 * Scan Read/Write 64 Ptr Simple/Unroll Loop.
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

typedef std::pair<uint64_t,uint64_t> uint128;

// 128-bit writer in a simple loop (C version)
void cScanWrite128PtrSimpleLoop(void* memarea, size_t size, size_t repeats)
{
    uint128* begin = (uint128*)memarea;
    uint128* end = begin + size / sizeof(uint128);
    uint64_t val64 = 0xC0FFEEEEBABE0000;
    uint128 value = uint128(val64,val64);

    do {
        uint128* p = begin;
        do {
            *p++ = value;
        }
        while(p < end);
    }
    while (--repeats != 0);
}

//REGISTER(cScanWrite128PtrSimpleLoop, 16, 16);

// 128-bit writer in a simple loop (Assembler version)
void ScanWrite128PtrSimpleLoop(void* memarea, size_t size, size_t repeats)
{
    asm("mov    $0xC0FFEEEEBABE0000, %%rax \n"
        "movq   %%rax, %%xmm0 \n"
        "movq   %%rax, %%xmm1 \n"
        "movlhps %%xmm0, %%xmm1 \n"     // xmm0 = test value
        "mov    %[memarea], %%rsi \n"   // rsi = memarea
        "mov    %[size], %%rdi \n"
        "add    %%rsi, %%rdi \n"        // rdi = memarea+size
        "1: \n" // start of repeat loop
        "mov    %%rsi, %%rax \n"        // rax = reset loop iterator
        "2: \n" // start of write loop
        "movdqa %%xmm0, (%%rax) \n"
        "add    $16, %%rax \n"
        // test write loop condition
        "cmp    %%rdi, %%rax \n"        // compare to end iterator
        "jb     2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        :
        : [memarea] "g" (memarea), [size] "g" (size), [repeats] "r" (repeats)
        : "rax", "rsi", "rdi", "xmm0", "xmm1");
}

REGISTER(ScanWrite128PtrSimpleLoop, 16, 16);

// 128-bit writer in an unrolled loop (Assembler version)
void ScanWrite128PtrUnrollLoop(void* memarea, size_t size, size_t repeats)
{
    asm("mov    $0xC0FFEEEEBABE0000, %%rax \n"
        "movq   %%rax, %%xmm0 \n"
        "movq   %%rax, %%xmm1 \n"
        "movlhps %%xmm0, %%xmm1 \n"     // xmm0 = test value
        "mov    %[memarea], %%rsi \n"   // rsi = memarea
        "mov    %[size], %%rdi \n"
        "add    %%rsi, %%rdi \n"        // rdi = memarea+size
        "1: \n" // start of repeat loop
        "mov    %%rsi, %%rax \n"        // rax = reset loop iterator
        "2: \n" // start of write loop
        "movdqa %%xmm0, 0*16(%%rax) \n"
        "movdqa %%xmm0, 1*16(%%rax) \n"
        "movdqa %%xmm0, 2*16(%%rax) \n"
        "movdqa %%xmm0, 3*16(%%rax) \n"
        "movdqa %%xmm0, 4*16(%%rax) \n"
        "movdqa %%xmm0, 5*16(%%rax) \n"
        "movdqa %%xmm0, 6*16(%%rax) \n"
        "movdqa %%xmm0, 7*16(%%rax) \n"
        "movdqa %%xmm0, 8*16(%%rax) \n"
        "movdqa %%xmm0, 9*16(%%rax) \n"
        "movdqa %%xmm0, 10*16(%%rax) \n"
        "movdqa %%xmm0, 11*16(%%rax) \n"
        "movdqa %%xmm0, 12*16(%%rax) \n"
        "movdqa %%xmm0, 13*16(%%rax) \n"
        "movdqa %%xmm0, 14*16(%%rax) \n"
        "movdqa %%xmm0, 15*16(%%rax) \n"
        "add    $16*16, %%rax \n"
        // test write loop condition
        "cmp    %%rdi, %%rax \n"        // compare to end iterator
        "jb     2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        :
        : [memarea] "g" (memarea), [size] "g" (size), [repeats] "r" (repeats)
        : "rax", "rsi", "rdi", "xmm0", "xmm1");
}

REGISTER(ScanWrite128PtrUnrollLoop, 16, 16);

// 128-bit reader in a simple loop (Assembler version)
void ScanRead128PtrSimpleLoop(void* memarea, size_t size, size_t repeats)
{
    asm("mov    $0xC0FFEEEEBABE0000, %%rax \n"
        "movq   %%rax, %%xmm0 \n"
        "movq   %%rax, %%xmm1 \n"
        "movlhps %%xmm0, %%xmm1 \n"     // xmm0 = test value
        "mov    %[memarea], %%rsi \n"   // rsi = memarea
        "mov    %[size], %%rdi \n"
        "add    %%rsi, %%rdi \n"        // rdi = memarea+size
        "1: \n" // start of repeat loop
        "mov    %%rsi, %%rax \n"        // rax = reset loop iterator
        "2: \n" // start of read loop
        "movdqa (%%rax), %%xmm0 \n"
        "add    $16, %%rax \n"
        // test read loop condition
        "cmp    %%rdi, %%rax \n"        // compare to end iterator
        "jb     2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        :
        : [memarea] "g" (memarea), [size] "g" (size), [repeats] "r" (repeats)
        : "rax", "rsi", "rdi", "xmm0", "xmm1");
}

REGISTER(ScanRead128PtrSimpleLoop, 16, 16);

// 128-bit reader in an unrolled loop (Assembler version)
void ScanRead128PtrUnrollLoop(void* memarea, size_t size, size_t repeats)
{
    asm("mov    $0xC0FFEEEEBABE0000, %%rax \n"
        "movq   %%rax, %%xmm0 \n"
        "movq   %%rax, %%xmm1 \n"
        "movlhps %%xmm0, %%xmm1 \n"     // xmm0 = test value
        "mov    %[memarea], %%rsi \n"   // rsi = memarea
        "mov    %[size], %%rdi \n"
        "add    %%rsi, %%rdi \n"        // rdi = memarea+size
        "1: \n" // start of repeat loop
        "mov    %%rsi, %%rax \n"        // rax = reset loop iterator
        "2: \n" // start of read loop
        "movdqa 0*16(%%rax), %%xmm0 \n"
        "movdqa 1*16(%%rax), %%xmm0 \n"
        "movdqa 2*16(%%rax), %%xmm0 \n"
        "movdqa 3*16(%%rax), %%xmm0 \n"
        "movdqa 4*16(%%rax), %%xmm0 \n"
        "movdqa 5*16(%%rax), %%xmm0 \n"
        "movdqa 6*16(%%rax), %%xmm0 \n"
        "movdqa 7*16(%%rax), %%xmm0 \n"
        "movdqa 8*16(%%rax), %%xmm0 \n"
        "movdqa 9*16(%%rax), %%xmm0 \n"
        "movdqa 10*16(%%rax), %%xmm0 \n"
        "movdqa 11*16(%%rax), %%xmm0 \n"
        "movdqa 12*16(%%rax), %%xmm0 \n"
        "movdqa 13*16(%%rax), %%xmm0 \n"
        "movdqa 14*16(%%rax), %%xmm0 \n"
        "movdqa 15*16(%%rax), %%xmm0 \n"
        "add    $16*16, %%rax \n"
        // test read loop condition
        "cmp    %%rdi, %%rax \n"        // compare to end iterator
        "jb     2b \n"
        // test repeat loop condition
        "dec    %[repeats] \n"          // until repeats = 0
        "jnz    1b \n"
        :
        : [memarea] "g" (memarea), [size] "g" (size), [repeats] "r" (repeats)
        : "rax", "rsi", "rdi", "xmm0", "xmm1");
}

REGISTER(ScanRead128PtrUnrollLoop, 16, 16);

// -----------------------------------------------------------------------------
