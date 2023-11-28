#ifndef BITS_H
#define BITS_H
#include "third_party/python/Modules/_decimal/libmpdec/mpdecimal.h"

/*
 * Check if 𝑛 is a power of 2.
 */
static inline int
ispower2(mpd_size_t n)
{
    return n != 0 && (n & (n-1)) == 0;
}

/*
 * Returns most significant bit position of 𝑛.
 * Assumptions: 𝑛 ≠ 0
 */
static inline int
mpd_bsr(mpd_size_t n)
{
#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
    return __builtin_clzll(n) ^ (sizeof(long long) * CHAR_BIT - 1);
#else
    int pos = 0;
    mpd_size_t tmp;
    tmp = n >> 32; if (tmp != 0) { n = tmp; pos += 32; }
    tmp = n >> 16; if (tmp != 0) { n = tmp; pos += 16; }
    tmp = n >>  8; if (tmp != 0) { n = tmp; pos +=  8; }
    tmp = n >>  4; if (tmp != 0) { n = tmp; pos +=  4; }
    tmp = n >>  2; if (tmp != 0) { n = tmp; pos +=  2; }
    tmp = n >>  1; if (tmp != 0) { n = tmp; pos +=  1; }
    return pos + (int)n - 1;
#endif
}

/*
 * Return the least significant bit position of n from 0 to 31 (63).
 * Assumptions: n != 0.
 */
static inline int
mpd_bsf(mpd_size_t n)
{
#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
    return __builtin_ctzll(n);
#else
    int pos;
    pos = 63;
    if (n & 0x00000000FFFFFFFFULL) { pos -= 32; } else { n >>= 32; }
    if (n & 0x000000000000FFFFULL) { pos -= 16; } else { n >>= 16; }
    if (n & 0x00000000000000FFULL) { pos -=  8; } else { n >>=  8; }
    if (n & 0x000000000000000FULL) { pos -=  4; } else { n >>=  4; }
    if (n & 0x0000000000000003ULL) { pos -=  2; } else { n >>=  2; }
    if (n & 0x0000000000000001ULL) { pos -=  1; }
    return pos;
#endif
}

#endif /* BITS_H */
