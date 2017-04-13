/******************************************************************************
 * funcs_c.h
 *
 * All Test Functions in C code: they are codenamed as
 * Scan/Perm Read/Write 32/64/128/256 Ptr/Index Simple/Unroll Loop.
 *
 * Scan = consecutive scanning, Perm = walk permutation cycle.
 * Read/Write = obvious
 * 32/64/128/256 = size of access
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

#warning "The C implementations of these loops are subject to compiler optimizations of all kinds."
#warning "For getting reliable results it is recommended to replace them with inline assembly."

// ****************************************************************************
// ----------------------------------------------------------------------------
// 64-bit Operations
// ----------------------------------------------------------------------------
// ****************************************************************************

// 64-bit writer in a simple loop (C version)
void cScanWrite64PtrSimpleLoop(char* memarea, size_t size, size_t repeats)
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

REGISTER(cScanWrite64PtrSimpleLoop, 8, 8, 1);

// -----------------------------------------------------------------------------

// 64-bit writer in an indexed loop (C version)
void cScanWrite64IndexSimpleLoop(char* _memarea, size_t _size, size_t repeats)
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

REGISTER(cScanWrite64IndexSimpleLoop, 8, 8, 1);

// ****************************************************************************
// ----------------------------------------------------------------------------
// 128-bit Operations
// ----------------------------------------------------------------------------
// ****************************************************************************

// 128-bit writer in a simple loop (C version)
void cScanWrite128PtrSimpleLoop(char* memarea, size_t size, size_t repeats)
{
    typedef std::pair<uint64_t,uint64_t> uint128;

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

REGISTER(cScanWrite128PtrSimpleLoop, 16, 16, 1);

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

REGISTER(cScanWrite32PtrSimpleLoop, 4, 4, 1);

// -----------------------------------------------------------------------------

// 32-bit writer in an indexed loop (C version)
void cScanWrite32IndexSimpleLoop(char* _memarea, size_t _size, size_t repeats)
{
    uint32_t* memarea = (uint32_t*)_memarea;
    size_t size = _size / sizeof(uint32_t);
    uint32_t value = 0xC0FFEEEE;

    do {
        for (size_t i = 0; i < size; ++i)
            memarea[i] = value;
    }
    while (--repeats != 0);
}

REGISTER(cScanWrite32IndexSimpleLoop, 4, 4, 1);

// ****************************************************************************
// ----------------------------------------------------------------------------
// Permutation Walking
// ----------------------------------------------------------------------------
// ****************************************************************************

#ifndef __LP64__

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

REGISTER_PERM(cPermRead32SimpleLoop, /* bytes */ 4, /* unroll */ 1);

#else

// follow 64-bit permutation in a simple loop (C version)
void cPermRead64SimpleLoop(char* memarea, size_t, size_t repeats)
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

REGISTER_PERM(cPermRead64SimpleLoop, /* bytes */ 8, /* unroll */ 1);

#endif

// -----------------------------------------------------------------------------
