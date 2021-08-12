#ifndef BITS_H
#define BITS_H
#include "third_party/python/Modules/_decimal/libmpdec/mpdecimal.h"
/* clang-format off */

/* Check if n is a power of 2. */
static inline int
ispower2(mpd_size_t n)
{
    return n != 0 && (n & (n-1)) == 0;
}

#if defined(ANSI)
#error oh no
/*
 * Return the most significant bit position of n from 0 to 31 (63).
 * Assumptions: n != 0.
 */
static inline int
mpd_bsr(mpd_size_t n)
{
    int pos = 0;
    mpd_size_t tmp;

#ifdef CONFIG_64
    tmp = n >> 32;
    if (tmp != 0) { n = tmp; pos += 32; }
#endif
    tmp = n >> 16;
    if (tmp != 0) { n = tmp; pos += 16; }
    tmp = n >> 8;
    if (tmp != 0) { n = tmp; pos += 8; }
    tmp = n >> 4;
    if (tmp != 0) { n = tmp; pos += 4; }
    tmp = n >> 2;
    if (tmp != 0) { n = tmp; pos += 2; }
    tmp = n >> 1;
    if (tmp != 0) { n = tmp; pos += 1; }

    return pos + (int)n - 1;
}

/*
 * Return the least significant bit position of n from 0 to 31 (63).
 * Assumptions: n != 0.
 */
static inline int
mpd_bsf(mpd_size_t n)
{
    int pos;

#ifdef CONFIG_64
    pos = 63;
    if (n & 0x00000000FFFFFFFFULL) { pos -= 32; } else { n >>= 32; }
    if (n & 0x000000000000FFFFULL) { pos -= 16; } else { n >>= 16; }
    if (n & 0x00000000000000FFULL) { pos -=  8; } else { n >>=  8; }
    if (n & 0x000000000000000FULL) { pos -=  4; } else { n >>=  4; }
    if (n & 0x0000000000000003ULL) { pos -=  2; } else { n >>=  2; }
    if (n & 0x0000000000000001ULL) { pos -=  1; }
#else
    pos = 31;
    if (n & 0x000000000000FFFFUL) { pos -= 16; } else { n >>= 16; }
    if (n & 0x00000000000000FFUL) { pos -=  8; } else { n >>=  8; }
    if (n & 0x000000000000000FUL) { pos -=  4; } else { n >>=  4; }
    if (n & 0x0000000000000003UL) { pos -=  2; } else { n >>=  2; }
    if (n & 0x0000000000000001UL) { pos -=  1; }
#endif
    return pos;
}
/* END ANSI */

#elif defined(ASM)
/*
 * Bit scan reverse. Assumptions: a != 0.
 */
static inline int
mpd_bsr(mpd_size_t a)
{
    mpd_size_t retval;

    __asm__ (
#ifdef CONFIG_64
        "bsrq %1, %0\n\t"
#else
        "bsr %1, %0\n\t"
#endif
        :"=r" (retval)
        :"r" (a)
        :"cc"
    );

    return (int)retval;
}

/*
 * Bit scan forward. Assumptions: a != 0.
 */
static inline int
mpd_bsf(mpd_size_t a)
{
    mpd_size_t retval;
    __asm__ (
#ifdef CONFIG_64
        "bsfq %1, %0\n\t"
#else
        "bsf %1, %0\n\t"
#endif
        :"=r" (retval)
        :"r" (a)
        :"cc"
    );
    return (int)retval;
}
/* END ASM */

#elif defined(MASM)
#include <intrin.h>
/*
 * Bit scan reverse. Assumptions: a != 0.
 */
static inline int __cdecl
mpd_bsr(mpd_size_t a)
{
    unsigned long retval;

#ifdef CONFIG_64
    _BitScanReverse64(&retval, a);
#else
    _BitScanReverse(&retval, a);
#endif

    return (int)retval;
}

/*
 * Bit scan forward. Assumptions: a != 0.
 */
static inline int __cdecl
mpd_bsf(mpd_size_t a)
{
    unsigned long retval;

#ifdef CONFIG_64
    _BitScanForward64(&retval, a);
#else
    _BitScanForward(&retval, a);
#endif

    return (int)retval;
}
/* END MASM (_MSC_VER) */
#else
  #error "missing preprocessor definitions"
#endif /* BSR/BSF */


#endif /* BITS_H */



