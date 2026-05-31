"""Framework classes for generation of bignum mod test cases."""
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#

from typing import Dict, List

from . import test_data_generation
from . import bignum_common
from .bignum_data import ONLY_PRIME_MODULI

class BignumModTarget(test_data_generation.BaseTarget):
    #pylint: disable=abstract-method, too-few-public-methods
    """Target for bignum mod test case generation."""
    target_basename = 'test_suite_bignum_mod.generated'


class BignumModMul(bignum_common.ModOperationCommon,
                   BignumModTarget):
    # pylint:disable=duplicate-code
    """Test cases for bignum mpi_mod_mul()."""
    symbol = "*"
    test_function = "mpi_mod_mul"
    test_name = "mbedtls_mpi_mod_mul"
    input_style = "arch_split"
    arity = 2

    def arguments(self) -> List[str]:
        return [self.format_result(self.to_montgomery(self.int_a)),
                self.format_result(self.to_montgomery(self.int_b)),
                bignum_common.quote_str(self.arg_n)
               ] + self.result()

    def result(self) -> List[str]:
        result = (self.int_a * self.int_b) % self.int_n
        return [self.format_result(self.to_montgomery(result))]


class BignumModSub(bignum_common.ModOperationCommon, BignumModTarget):
    """Test cases for bignum mpi_mod_sub()."""
    symbol = "-"
    test_function = "mpi_mod_sub"
    test_name = "mbedtls_mpi_mod_sub"
    input_style = "fixed"
    arity = 2

    def result(self) -> List[str]:
        result = (self.int_a - self.int_b) % self.int_n
        # To make negative tests easier, append 0 for success to the
        # generated cases
        return [self.format_result(result), "0"]

class BignumModInvNonMont(bignum_common.ModOperationCommon, BignumModTarget):
    """Test cases for bignum mpi_mod_inv() - not in Montgomery form."""
    moduli = ONLY_PRIME_MODULI  # for now only prime moduli supported
    symbol = "^ -1"
    test_function = "mpi_mod_inv_non_mont"
    test_name = "mbedtls_mpi_mod_inv non-Mont. form"
    input_style = "fixed"
    arity = 1
    suffix = True
    disallow_zero_a = True

    def result(self) -> List[str]:
        result = bignum_common.invmod_positive(self.int_a, self.int_n)
        # To make negative tests easier, append 0 for success to the
        # generated cases
        return [self.format_result(result), "0"]

class BignumModInvMont(bignum_common.ModOperationCommon, BignumModTarget):
    """Test cases for bignum mpi_mod_inv() - Montgomery form."""
    moduli = ONLY_PRIME_MODULI  # for now only prime moduli supported
    symbol = "^ -1"
    test_function = "mpi_mod_inv_mont"
    test_name = "mbedtls_mpi_mod_inv Mont. form"
    input_style = "arch_split"  # Mont. form requires arch_split
    arity = 1
    suffix = True
    disallow_zero_a = True
    montgomery_form_a = True

    def result(self) -> List[str]:
        result = bignum_common.invmod_positive(self.int_a, self.int_n)
        mont_result = self.to_montgomery(result)
        # To make negative tests easier, append 0 for success to the
        # generated cases
        return [self.format_result(mont_result), "0"]


class BignumModAdd(bignum_common.ModOperationCommon, BignumModTarget):
    """Test cases for bignum mpi_mod_add()."""
    count = 0
    symbol = "+"
    test_function = "mpi_mod_add"
    test_name = "mbedtls_mpi_mod_add"
    input_style = "fixed"

    def result(self) -> List[str]:
        result = (self.int_a + self.int_b) % self.int_n
        # To make negative tests easier, append "0" for success to the
        # generated cases
        return [self.format_result(result), "0"]
