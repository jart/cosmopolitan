# Copyright (c) The mlkem-native project authors
# Copyright (c) The mldsa-native project authors
# SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#
# The purpose of this script is to provide either brute-force proof
# or empirical evidence to arithmetic bounds for the modular
# arithmetic primitives used in this repository.
#

import random
from functools import lru_cache
from fractions import Fraction
from math import ceil

# Global constants
R = 2**32
Q = 8380417
Qinv = pow(Q, -1, R)
NQinv = pow(-Q, -1, R)


#
# Barrett multiplication via doubling
#


def round_even(x):
    return 2 * round(x / 2)


@lru_cache(maxsize=None)
def barrett_twiddle(b):
    """Compute twiddle required for Barrett multiplications
    via doubling-high-multiply."""
    return round_even(b * R / Q) // 2


def sqrdmulh_i32(a, b):
    """Doubling multiply high with rounding"""
    # We cannot use round() here because of its behaviour
    # on multiples of 0.5: round(-.5) = round(0.5) = 0
    return (2 * a * b + 2**31) // 2**32


def barmul(a, b):
    """Compute doubling Barrett multiplication of a and b"""
    b_twiddle = barrett_twiddle(b)
    return a * b - Q * sqrdmulh_i32(a, b_twiddle)


#
# Montgomery multiplication
#


def lift_signed_i32(x):
    """Returns signed canonical representative modulo R=2^32."""
    x = x % R
    if x >= R // 2:
        x -= R
    return x


@lru_cache(maxsize=None)
def montmul_neg_twiddle(b):
    return (b * NQinv) % R


@lru_cache(maxsize=None)
def montmul_pos_twiddle(b):
    return (b * Qinv) % R


def montmul_neg(a, b):
    b_twiddle = montmul_neg_twiddle(b)
    return (a * b + Q * lift_signed_i32(a * b_twiddle)) // R


def montmul_pos(a, b):
    b_twiddle = montmul_pos_twiddle(b)
    return (a * b - Q * lift_signed_i32(a * b_twiddle)) // R


#
# Generic test functions
#


def test_random(f, test_name, num_tests=10000000, bound_a=R // 2, bound_b=Q // 2):
    print(f"Randomly checking {test_name} ({num_tests} tests)...")
    for i in range(num_tests):
        if i % 100000 == 0:
            print(f"... run {i} tests ({((i * 1000) // num_tests)/10}%)")
        a = random.randrange(-bound_a, bound_a)
        b = random.randrange(-bound_b, bound_b)
        f(a, b)


#
# Test bound on Barrett multiplication, as used in AArch64 [I]NTT
#

"""
For Barrett multiplication implemented with sqrdmulh, Corollary 2 of @[NeonNTT]
shows the bound

  |barmul(a, b)| <= |a| |bR mod⟦⟧ q| / R + q / 2,

where ⟦⟧ denotes round-to-nearest-even-integer approximation (thus
|bR mod⟦⟧ q| < q). In particular, knowing that a fits inside int32_t (thus
|a| <= R/2) already implies |barmul(a, b)| < q.
"""


@lru_cache(maxsize=None)
def modq_even(a):
    return a - Q * round_even(a / Q)


def barmul_bound(a, b):
    bp = modq_even(b * R)
    return Fraction(abs(a) * abs(bp), R) + Fraction(Q, 2)


def barmul_bound_test(a, b):
    ab = barmul(a, b)
    bound = barmul_bound(a, b)
    if abs(ab) > bound:
        print(f"barmul_bound_test failure for (a,b)={(a,b)}")
        print(f"barmul(a,b): {ab}")
        print(f"bound: {bound}")
        assert False


def barmul_bound_test_random():
    test_random(
        barmul_bound_test,
        "bound on Barrett multiplication, as used in AArch64 [I]NTT",
    )


def barmul_bound_test_exceeds_3_quarters():
    """
    This example shows an extreme but legitimate case where the Barrett
    multiplication in the last butterfly of AArch64 INTT computes an output with
    magnitude larger than ceil(3q/4).

    The specific zeta used in that butterly can be found at the 32nd to last
    entry of the array mld_aarch64_intt_zetas_layer123456[] in
    dev/aarch64_clean/src/aarch64_zetas.c.
    """
    zeta = -294725
    a_worst = -2145337212
    a_zeta_worst = 6390331
    assert abs(a_worst) <= 256 * (Q - 1)
    assert barmul(a_worst, zeta) == a_zeta_worst
    assert abs(a_zeta_worst) > ceil(Fraction(3 * Q, 4))


barmul_bound_test_random()
barmul_bound_test_exceeds_3_quarters()


#
# Test bound on "Montgomery multiplication with signed canonical constant", as
# used in AVX2 [I]NTT
#

"""
In @[Survey_Hwang23, Section 2.2], the author noted the bound*

  |montmul(a, b)| <= (q/2) (1 + |a|/R).

In particular, knowing that a fits inside int32_t (thus |a| <= R/2) already
implies |montmul(a, b)| <= 3q/4 < ceil(3q/4).

(*) Strictly speaking, they considered the negative/additive variant
    montmul_neg(a, b), but the exact same bound and proof also work for the
    positive/subtractive variant montmul_pos(a, b).
"""


def montmul_pos_const_bound(a):
    return Fraction(Q, 2) * (1 + Fraction(abs(a), R))


def montmul_pos_const_bound_test(a, b):
    ab = montmul_pos(a, b)
    bound = montmul_pos_const_bound(a)
    if abs(ab) > bound:
        print(f"montmul_pos_const_bound_test failure for (a,b)={(a,b)}")
        print(f"montmul_pos(a,b): {ab}")
        print(f"bound: {bound}")
        assert False


def montmul_pos_const_bound_test_random():
    test_random(
        montmul_pos_const_bound_test,
        "bound on Montgomery multiplication with constant, as used in AVX2 [I]NTT",
    )


def montmul_pos_const_bound_tight():
    """
    This example shows that, unless we know more about a or b, the bound
    |montmul(a, b)| < MONTMUL_BOUND := ceil(3q/4) is the tightest exclusive
    bound.
    """
    a_worst = -R // 2
    b_worst = -(Q - 3) // 2
    ab_worst = montmul_pos(a_worst, b_worst)
    bound = ceil(Fraction(3 * Q, 4))
    assert ab_worst == bound - 1


montmul_pos_const_bound_test_random()
montmul_pos_const_bound_tight()
