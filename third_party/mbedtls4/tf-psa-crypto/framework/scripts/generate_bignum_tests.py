#!/usr/bin/env python3
"""Generate test data for bignum functions.

With no arguments, generate all test data. With non-option arguments,
generate only the specified files.

Class structure:

Child classes of test_data_generation.BaseTarget (file targets) represent an output
file. These indicate where test cases will be written to, for all subclasses of
this target. Multiple file targets should not reuse a `target_basename`.

Each subclass derived from a file target can either be:
  - A concrete class, representing a test function, which generates test cases.
  - An abstract class containing shared methods and attributes, not associated
        with a test function. An example is BignumOperation, which provides
        common features used for bignum binary operations.

Both concrete and abstract subclasses can be derived from, to implement
additional test cases (see BignumCmp and BignumCmpAbs for examples of deriving
from abstract and concrete classes).


Adding test case generation for a function:

A subclass representing the test function should be added, deriving from a
file target such as BignumTarget. This test class must set/implement the
following:
  - test_function: the function name from the associated .function file.
  - test_name: a descriptive name or brief summary to refer to the test
        function.
  - arguments(): a method to generate the list of arguments required for the
        test_function.
  - generate_function_tests(): a method to generate TestCases for the function.
        This should create instances of the class with required input data, and
        call `.create_test_case()` to yield the TestCase.

Additional details and other attributes/methods are given in the documentation
of BaseTarget in test_data_generation.py.
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

import sys
import math

from abc import ABCMeta
from typing import List

from mbedtls_framework import test_data_generation
from mbedtls_framework import bignum_common
# Import modules containing additional test classes
# Test function classes in these modules will be registered by
# the framework
from mbedtls_framework import bignum_core, bignum_mod_raw, bignum_mod # pylint: disable=unused-import

class BignumTarget(test_data_generation.BaseTarget):
    #pylint: disable=too-few-public-methods
    """Target for bignum (legacy) test case generation."""
    target_basename = 'test_suite_bignum.generated'


class BignumOperation(bignum_common.OperationCommon, BignumTarget,
                      metaclass=ABCMeta):
    #pylint: disable=abstract-method
    """Common features for bignum operations in legacy tests."""
    unique_combinations_only = True
    input_values = [
        "", "0", "-", "-0",
        "7b", "-7b",
        "0000000000000000123", "-0000000000000000123",
        "1230000000000000000", "-1230000000000000000"
    ]

    def description_suffix(self) -> str:
        #pylint: disable=no-self-use # derived classes need self
        """Text to add at the end of the test case description."""
        return ""

    def description(self) -> str:
        """Generate a description for the test case.

        If not set, case_description uses the form A `symbol` B, where symbol
        is used to represent the operation. Descriptions of each value are
        generated to provide some context to the test case.
        """
        if not self.case_description:
            self.case_description = "{} {} {}".format(
                self.value_description(self.arg_a),
                self.symbol,
                self.value_description(self.arg_b)
            )
            description_suffix = self.description_suffix()
            if description_suffix:
                self.case_description += " " + description_suffix
        return super().description()

    @staticmethod
    def value_description(val) -> str:
        """Generate a description of the argument val.

        This produces a simple description of the value, which is used in test
        case naming to add context.
        """
        if val == "":
            return "0 (null)"
        if val == "-":
            return "negative 0 (null)"
        if val == "0":
            return "0 (1 limb)"

        if val[0] == "-":
            tmp = "negative"
            val = val[1:]
        else:
            tmp = "positive"
        if val[0] == "0":
            tmp += " with leading zero limb"
        elif len(val) > 10:
            tmp = "large " + tmp
        return tmp


class BignumGCDInvModOperation(BignumOperation):
    #pylint: disable=abstract-method
    """Common features for testing GCD and Invmod functions."""
    def __init__(self, val_a: str, val_b: str) -> None:
        super().__init__(val_a=val_a, val_b=val_b)

    def description_suffix(self) -> str:
        comparison_symbol = '='
        if abs(self.int_a) > abs(self.int_b):
            comparison_symbol = '>'
        elif abs(self.int_a) < abs(self.int_b):
            comparison_symbol = '<'
        suffix_parts = [
            f"|A|{comparison_symbol}|N|",
            *(["A<0"] if self.int_a < 0 else []),
            *(["N<0"] if self.int_b < 0 else []),
            "A=0" if self.int_a == 0 else f"A {'even' if self.int_a % 2 == 0 else 'odd'}",
            "N=0" if self.int_b == 0 else f"B {'even' if self.int_b % 2 == 0 else 'odd'}"
        ]
        return ": " + ", ".join(suffix_parts)

    # The default values from BignumOperation are not useful, so overwrite them.
    input_values = [
        "c79e27fc71c69a08b3e85bd48b9cd3be9aa8e2e56df39f4ed8",
        "299dd34be98436729eb10f690f8d2bfc5bee21984b775e1e75",
        "-ecbb3a4e986d488172ecd54f7bd71bd18050c4ed",
        "7da9ec44f42e6311c56a",
        "cdbcce3f763819345cfb",
        "100000000", "300000000", "500000000",
        "50000", "30000",
        "1", "2", "3", "", "00", "-1"
    ]
    input_cases = [
        ("bc7fa9fb389618302e8b", "d49730e586607d42269f"),
        ("28bcc01a2d54b174532e", "d1915057d829a934c25d"),
        ("d56b50834719280dfa1d", "f007b78f6278ebcccd57"),
        ("8c327d1d8743c89d4483", "aa20b0c1f97a428311b5"),
        ("e905382f38", "c844b4f9bdaa5ed0002df3dbd2991cd9b9d"),
        ("e4623ef13d", "f2a4894ede013e354e481fe8974e67"),
        ("9f6afa8bdb", "b50aa03a7066df6f27bd6267b"),
        ("95f99b7122", "e8c74031ec75839f7539"),
        ("32", "948fbec067"),
        ("7445", "948fbec067"),
        ("31850e", "948fbec067"),
        ("421c2cc8", "948fbec067"),
        ("32a69", "71e107"),
        ("36d4e9", "3e05d1"),
        ("babf01", "1bf699d1"),
        ("7", "31"),
    ]

    def get_return_code_gcd_modinv_odd_gcd_only(self) -> str:
        code = "0"
        if (self.int_a > self.int_b) or \
           (self.int_a < 0) or \
           (self.int_b % 2 == 0):
            code = "MBEDTLS_ERR_MPI_BAD_INPUT_DATA"
        return code

    def get_return_code_gcd_modinv_odd(self) -> str:
        if self.int_b < 2:
            return "MBEDTLS_ERR_MPI_BAD_INPUT_DATA"
        return self.get_return_code_gcd_modinv_odd_gcd_only()


class BignumCmp(BignumOperation):
    """Test cases for bignum value comparison."""
    count = 0
    test_function = "mpi_cmp_mpi"
    test_name = "MPI compare"
    input_cases = [
        ("-2", "-3"),
        ("-2", "-2"),
        ("2b4", "2b5"),
        ("2b5", "2b6")
        ]

    def __init__(self, val_a, val_b) -> None:
        super().__init__(val_a, val_b)
        self._result = int(self.int_a > self.int_b) - int(self.int_a < self.int_b)
        self.symbol = ["<", "==", ">"][self._result + 1]

    def result(self) -> List[str]:
        return [str(self._result)]


class BignumCmpAbs(BignumCmp):
    """Test cases for absolute bignum value comparison."""
    count = 0
    test_function = "mpi_cmp_abs"
    test_name = "MPI compare (abs)"

    def __init__(self, val_a, val_b) -> None:
        super().__init__(val_a.strip("-"), val_b.strip("-"))


class BignumInvMod(BignumOperation):
    """Test cases for bignum modular inverse."""
    count = 0
    symbol = "^-1 mod"
    test_function = "mpi_inv_mod"
    test_name = "MPI inv_mod"
    # The default values are not very useful here, so clear them.
    input_values = [] # type: List[str]
    input_cases = bignum_common.combination_two_lists(
        # Input values for A
        bignum_common.expand_list_negative([
            "aa4df5cb14b4c31237f98bd1faf527c283c2d0f3eec89718664ba33f9762907c",
            "f847e7731a2687c837f6b825f2937d997bf66814d3db79b27b",
            "2ec0888f",
            "22fbdf4c",
            "32cf9a75",
        ]),
        # Input values for N - must be positive.
        [
            "fffbbd660b94412ae61ead9c2906a344116e316a256fd387874c6c675b1d587d",
            "2fe72fa5c05bc14c1279e37e2701bd956822999f42c5cbe84",
            "2ec0888f",
            "22fbdf4c",
            "34d0830",
            "364b6729",
            "14419cd",
        ],
    )

    def __init__(self, val_a: str, val_b: str) -> None:
        super().__init__(val_a, val_b)
        if math.gcd(self.int_a, self.int_b) == 1:
            self._result = bignum_common.invmod_positive(self.int_a, self.int_b)
        else:
            self._result = -1 # No modular inverse.

    def description_suffix(self) -> str:
        suffix = ": "
        # Assuming N (int_b) is always positive, compare absolute values,
        # but only print the absolute value bars when A is negative.
        a_str = "A" if (self.int_a >= 0) else "|A|"
        if abs(self.int_a) > self.int_b:
            suffix += f"{a_str}>N"
        elif abs(self.int_a) < self.int_b:
            suffix += f"{a_str}<N"
        else:
            suffix += f"{a_str}=N"
        if self.int_a < 0:
            suffix += ", A<0"
        if self._result == -1:
            suffix += ", no inverse"
        return suffix

    def result(self) -> List[str]:
        if self._result == -1: # No modular inverse.
            return [bignum_common.quote_str("0"), "MBEDTLS_ERR_MPI_NOT_ACCEPTABLE"]
        return [bignum_common.quote_str("{:x}".format(self._result)), "0"]


class BignumGCD(BignumOperation):
    """Test cases for greatest common divisor."""
    count = 0
    symbol = "GCD"
    test_function = "mpi_gcd"
    test_name = "GCD"
    # The default values are not very useful here, so overwrite them.
    input_values = bignum_common.expand_list_negative([
        "3c094fd6b36ee4902c8ba84d13a401def90a2130116dad3361",
        "b2b06ebe14a185a83d5d2d7bddd1dd0e05e800d6b914fbed4e",
        "203265b387",
        "9bc8e63852",
        "100000000",
        "300000000",
        "500000000",
        "50000",
        "30000",
        "1",
        "2",
        "3",
        ])

    def __init__(self, val_a: str, val_b: str) -> None:
        super().__init__(val_a, val_b)
        # We always expect a positive result as the test data
        # does not contain zero.
        self._result = math.gcd(self.int_a, self.int_b)

    def description_suffix(self) -> str:
        suffix = ": "
        if abs(self.int_a) > abs(self.int_b):
            suffix += "|A|>|B|"
        elif abs(self.int_a) < abs(self.int_b):
            suffix += "|A|<|B|"
        else:
            suffix += "|A|=|B|"
        if self.int_a < 0:
            suffix += ", A<0"
        if self.int_b < 0:
            suffix += ", B<0"
        suffix += ", A even" if (self.int_a % 2 == 0) else ", A odd"
        suffix += ", B even" if (self.int_b % 2 == 0) else ", B odd"
        return suffix

    def result(self) -> List[str]:
        return [bignum_common.quote_str("{:x}".format(self._result))]


class BignumGCDModInvOdd(BignumGCDInvModOperation):
    """Test cases for both modular inverse and greatest common divisor."""
    count = 0
    symbol = "GCD & ^-1 mod"
    test_function = "mpi_gcd_modinv_odd_both"
    test_name = "GCD & mod inv"

    def __init__(self, val_a: str, val_b: str) -> None:
        super().__init__(val_a, val_b)
        self._result_code = self.get_return_code_gcd_modinv_odd()
        self._result_gcd = math.gcd(self.int_a, self.int_b)
        # Only compute the modular inverse if we will get a result - negative
        # and zero Ns are also present in the test data so skip them too.
        if self._result_gcd == 1 and self.int_b > 1:
            self._result_invmod = \
            bignum_common.invmod_positive(self.int_a, self.int_b) # type: int | None
        else:
            self._result_invmod = None # No inverse

    def result(self) -> List[str]:
        # The test requires us to tell it if there is no modular inverse.
        if self._result_invmod is None:
            result_invmod = "no_inverse"
        else:
            result_invmod = "{:x}".format(self._result_invmod)
        return [
            self.format_result(self._result_gcd),
            bignum_common.quote_str(result_invmod),
            self._result_code,
        ]


class BignumGCDModInvOddOnlyGCD(BignumGCDInvModOperation):
    """Test cases for greatest common divisor only."""
    count = 0
    symbol = "GCD"
    test_function = "mpi_gcd_modinv_odd_only_gcd"
    test_name = "GCD only"

    def __init__(self, val_a: str, val_b: str) -> None:
        super().__init__(val_a, val_b)
        self._result_code = self.get_return_code_gcd_modinv_odd_gcd_only()
        # We always expect a positive result as the function should reject
        # negative inputs.
        self._result_gcd = math.gcd(self.int_a, self.int_b)

    def result(self) -> List[str]:
        return [self.format_result(self._result_gcd), self._result_code]


class BignumGCDModInvOddOnlyModInv(BignumGCDInvModOperation):
    """Test cases for modular inverse only."""
    count = 0
    symbol = "^-1 mod"
    test_function = "mpi_gcd_modinv_odd_only_modinv"
    test_name = "Mod inv only"

    def __init__(self, val_a: str, val_b: str) -> None:
        super().__init__(val_a, val_b)
        self._result_code = self.get_return_code_gcd_modinv_odd()
        # Only compute the modular inverse if we will get a result - negative
        # and zero Ns are also present in the test data so skip them too.
        if math.gcd(self.int_a, self.int_b) == 1 and self.int_b > 1:
            self._result_invmod = \
            bignum_common.invmod_positive(self.int_a, self.int_b) # type: int | None
        else:
            self._result_invmod = None # No inverse

    def result(self) -> List[str]:
        # The test requires us to tell it if there is no modular inverse.
        if self._result_invmod is None:
            return [bignum_common.quote_str("no_inverse"), self._result_code]
        return [self.format_result(self._result_invmod), self._result_code]


class BignumAdd(BignumOperation):
    """Test cases for bignum value addition."""
    count = 0
    symbol = "+"
    test_function = "mpi_add_mpi"
    test_name = "MPI add"
    input_cases = bignum_common.combination_pairs(
        [
            "1c67967269c6", "9cde3",
            "-1c67967269c6", "-9cde3",
        ]
    )

    def __init__(self, val_a: str, val_b: str) -> None:
        super().__init__(val_a, val_b)
        self._result = self.int_a + self.int_b

    def description_suffix(self) -> str:
        if (self.int_a >= 0 and self.int_b >= 0):
            return "" # obviously positive result or 0
        if (self.int_a <= 0 and self.int_b <= 0):
            return "" # obviously negative result or 0
        # The sign of the result is not obvious, so indicate it
        return ", result{}0".format('>' if self._result > 0 else
                                    '<' if self._result < 0 else '=')

    def result(self) -> List[str]:
        return [bignum_common.quote_str("{:x}".format(self._result))]

if __name__ == '__main__':
    # Use the section of the docstring relevant to the CLI as description
    test_data_generation.main(sys.argv[1:], "\n".join(__doc__.splitlines()[:4]))
