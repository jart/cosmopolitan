#ifndef UMODARITH_H
#define UMODARITH_H
#include "third_party/python/Modules/_decimal/libmpdec/constants.h"
#include "third_party/python/Modules/_decimal/libmpdec/mpdecimal.h"
#include "third_party/python/Modules/_decimal/libmpdec/typearith.h"

/* Bignum: Low level routines for unsigned modular arithmetic. These are
   used in the fast convolution functions for very large coefficients. */

/*
 * Restrictions: a < m and b < m
 * ACL2 proof: umodarith.lisp: addmod-correct
 */
static inline mpd_uint_t
addmod(mpd_uint_t a, mpd_uint_t b, mpd_uint_t m)
{
    mpd_uint_t s;
    s = a + b;
    s = (s < a) ? s - m : s;
    s = (s >= m) ? s - m : s;
    return s;
}

/*
 * Restrictions: a < m and b < m
 * ACL2 proof: umodarith.lisp: submod-2-correct
 */
static inline mpd_uint_t
submod(mpd_uint_t a, mpd_uint_t b, mpd_uint_t m)
{
    mpd_uint_t d;
    d = a - b;
    d = (a < b) ? d + m : d;
    return d;
}

/*
 * Restrictions: a < 2m and b < 2m
 * ACL2 proof: umodarith.lisp: section ext-submod
 */
static inline mpd_uint_t
ext_submod(mpd_uint_t a, mpd_uint_t b, mpd_uint_t m)
{
    mpd_uint_t d;
    a = (a >= m) ? a - m : a;
    b = (b >= m) ? b - m : b;
    d = a - b;
    d = (a < b) ? d + m : d;
    return d;
}

/*
 * Reduce double word modulo m.
 * Restrictions: m != 0
 * ACL2 proof: umodarith.lisp: section dw-reduce
 */
static inline mpd_uint_t
dw_reduce(mpd_uint_t hi, mpd_uint_t lo, mpd_uint_t m)
{
    mpd_uint_t r1, r2, w;
    _mpd_div_word(&w, &r1, hi, m);
    _mpd_div_words(&w, &r2, r1, lo, m);
    return r2;
}

/*
 * Subtract double word from a.
 * Restrictions: a < m
 * ACL2 proof: umodarith.lisp: section dw-submod
 */
static inline mpd_uint_t
dw_submod(mpd_uint_t a, mpd_uint_t hi, mpd_uint_t lo, mpd_uint_t m)
{
    mpd_uint_t d, r;
    r = dw_reduce(hi, lo, m);
    d = a - r;
    d = (a < r) ? d + m : d;
    return d;
}

/**
 * Calculates (a × b) % 𝑝 where 𝑝 is special
 *
 * In the whole comment, "⩭" stands for "is congruent with".
 *
 * Result of a × b in terms of high/low words:
 *
 *    (1) hi × 2⁶⁴ + lo = a × b
 *
 * Special primes:
 *
 *    (2) 𝑝 = 2⁶⁴ - z + 1, where z = 2ⁿ
 *
 *        i.e. 0xfffffffffffffc01
 *             0xfffffffffffff001
 *             0xffffffffff000001
 *             0xffffffff00000001
 *             0xfffffffc00000001
 *             0xffffff0000000001
 *             0xffffff0000000001
 *
 * Single step modular reduction:
 *
 *    (3) R(hi, lo) = hi × z - hi + lo
 *
 *
 * Strategy
 * --------
 *
 *    a) Set (hi, lo) to the result of a × b.
 *
 *    b) Set (hi′, lo′) to the result of R(hi, lo).
 *
 *    c) Repeat step b) until 0 ≤ hi′ × 2⁶⁴ + lo′ < 𝟸×𝑝.
 *
 *    d) If the result is less than 𝑝, return lo′. Otherwise return lo′ - 𝑝.
 *
 *
 * The reduction step b) preserves congruence
 * ------------------------------------------
 *
 *     hi × 2⁶⁴ + lo ⩭ hi × z - hi + lo   (mod 𝑝)
 *
 *     Proof:
 *     ~~~~~~
 *
 *        hi × 2⁶⁴ + lo = (2⁶⁴ - z + 1) × hi + z × hi - hi + lo
 *
 *                      = 𝑝 × hi             + z × hi - hi + lo
 *
 *                      ⩭ z × hi - hi + lo   (mod 𝑝)
 *
 *
 * Maximum numbers of step b)
 * --------------------------
 *
 * To avoid unnecessary formalism, define:
 *
 *     def R(hi, lo, z):
 *          return divmod(hi * z - hi + lo, 2**64)
 *
 * For simplicity, assume hi=2⁶⁴-1, lo=2⁶⁴-1 after the
 * initial multiplication a × b. This is of course impossible
 * but certainly covers all cases.
 *
 * Then, for p1:
 *
 *     z  = 2³²
 *     hi = 2⁶⁴-1
 *     lo = 2⁶⁴-1
 *     p1 = 2⁶⁴ - z + 1
 *     hi, lo = R(hi, lo, z)    # First reduction
 *     hi, lo = R(hi, lo, z)    # Second reduction
 *     hi × 2⁶⁴ + lo < 2 × p1   # True
 *
 * For p2:
 *
 *     z  = 2³⁴
 *     hi = 2⁶⁴-1
 *     lo = 2⁶⁴-1
 *     p2 = 2⁶⁴ - z + 1
 *     hi, lo = R(hi, lo, z)    # First reduction
 *     hi, lo = R(hi, lo, z)    # Second reduction
 *     hi, lo = R(hi, lo, z)    # Third reduction
 *     hi × 2⁶⁴ + lo < 2 × p2   # True
 *
 * For p3:
 *
 *     z  = 2⁴⁰
 *     hi = 2⁶⁴-1
 *     lo = 2⁶⁴-1
 *     p3 = 2⁶⁴ - z + 1
 *     hi, lo = R(hi, lo, z)    # First reduction
 *     hi, lo = R(hi, lo, z)    # Second reduction
 *     hi, lo = R(hi, lo, z)    # Third reduction
 *     hi × 2⁶⁴ + lo < 2 × p3   # True
 *
 * Step d) preserves congruence and yields a result < 𝑝
 * ----------------------------------------------------
 *
 * Case hi = 0:
 *
 *   Case lo < 𝑝: trivial.
 *
 *   Case lo ≥ 𝑝:
 *
 *     lo ⩭ lo - 𝑝   (mod 𝑝)             # result is congruent
 *
 *     𝑝 ≤ lo < 𝟸×𝑝  →  0 ≤ lo - 𝑝 < 𝑝   # result is in the correct range
 *
 * Case hi = 1:
 *
 *     𝑝 < 2⁶⁴ Λ 2⁶⁴ + lo < 𝟸×𝑝  →  lo < 𝑝   # lo is always less than 𝑝
 *
 *     2⁶⁴ + lo ⩭ 2⁶⁴ + (lo - 𝑝)    (mod 𝑝)  # result is congruent
 *
 *              = lo - 𝑝   # exactly the same value as the previous RHS
 *                         # in uint64_t arithmetic.
 *
 *     𝑝 < 2⁶⁴ + lo < 𝟸×𝑝  →  0 < 2⁶⁴ + (lo - 𝑝) < 𝑝  # correct range
 *
 *
 * [1] http://www.apfloat.org/apfloat/2.40/apfloat.pdf
 */
static inline mpd_uint_t
x64_mulmod(mpd_uint_t a, mpd_uint_t b, mpd_uint_t m)
{
    mpd_uint_t hi, lo, x, y;
    _mpd_mul_words(&hi, &lo, a, b);
    if (m & (1ULL<<32)) { /* P1 */
        /* first reduction */
        x = y = hi;
        hi >>= 32;
        x = lo - x;
        if (x > lo) hi--;
        y <<= 32;
        lo = y + x;
        if (lo < y) hi++;
        /* second reduction */
        x = y = hi;
        hi >>= 32;
        x = lo - x;
        if (x > lo) hi--;
        y <<= 32;
        lo = y + x;
        if (lo < y) hi++;
        return hi || lo >= m ? lo - m : lo;
    }
    else if (m & (1ULL<<34)) { /* P2 */
        /* first reduction */
        x = y = hi;
        hi >>= 30;
        x = lo - x;
        if (x > lo) hi--;
        y <<= 34;
        lo = y + x;
        if (lo < y) hi++;
        /* second reduction */
        x = y = hi;
        hi >>= 30;
        x = lo - x;
        if (x > lo) hi--;
        y <<= 34;
        lo = y + x;
        if (lo < y) hi++;
        /* third reduction */
        x = y = hi;
        hi >>= 30;
        x = lo - x;
        if (x > lo) hi--;
        y <<= 34;
        lo = y + x;
        if (lo < y) hi++;
        return hi || lo >= m ? lo - m : lo;
    }
    else { /* P3 */
        /* first reduction */
        x = y = hi;
        hi >>= 24;
        x = lo - x;
        if (x > lo) hi--;
        y <<= 40;
        lo = y + x;
        if (lo < y) hi++;
        /* second reduction */
        x = y = hi;
        hi >>= 24;
        x = lo - x;
        if (x > lo) hi--;
        y <<= 40;
        lo = y + x;
        if (lo < y) hi++;
        /* third reduction */
        x = y = hi;
        hi >>= 24;
        x = lo - x;
        if (x > lo) hi--;
        y <<= 40;
        lo = y + x;
        if (lo < y) hi++;
        return hi || lo >= m ? lo - m : lo;
    }
}

static inline void
x64_mulmod2c(mpd_uint_t *a, mpd_uint_t *b, mpd_uint_t w, mpd_uint_t m)
{
    *a = x64_mulmod(*a, w, m);
    *b = x64_mulmod(*b, w, m);
}

static inline void
x64_mulmod2(mpd_uint_t *a0, mpd_uint_t b0, mpd_uint_t *a1, mpd_uint_t b1,
            mpd_uint_t m)
{
    *a0 = x64_mulmod(*a0, b0, m);
    *a1 = x64_mulmod(*a1, b1, m);
}

static inline mpd_uint_t
x64_powmod(mpd_uint_t base, mpd_uint_t exp, mpd_uint_t umod)
{
    mpd_uint_t r = 1;
    while (exp > 0) {
        if (exp & 1)
            r = x64_mulmod(r, base, umod);
        base = x64_mulmod(base, base, umod);
        exp >>= 1;
    }
    return r;
}

#endif /* UMODARITH_H */
