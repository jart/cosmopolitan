"""Framework classes for generation of bignum core test cases."""
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#

import math
import random

from typing import Dict, Iterator, List, Tuple

from . import test_case
from . import test_data_generation
from . import bignum_common
from . import bignum_data

class BignumCoreTarget(test_data_generation.BaseTarget):
    #pylint: disable=abstract-method, too-few-public-methods
    """Target for bignum core test case generation."""
    target_basename = 'test_suite_bignum_core.generated'


class BignumCoreShiftR(BignumCoreTarget, test_data_generation.BaseTest):
    """Test cases for mbedtls_bignum_core_shift_r()."""
    count = 0
    test_function = "mpi_core_shift_r"
    test_name = "Core shift right"

    DATA = [
        ('00', '0', [0, 1, 8]),
        ('01', '1', [0, 1, 2, 8, 64]),
        ('dee5ca1a7ef10a75', '64-bit',
         list(range(11)) + [31, 32, 33, 63, 64, 65, 71, 72]),
        ('002e7ab0070ad57001', '[leading 0 limb]',
         [0, 1, 8, 63, 64]),
        ('a1055eb0bb1efa1150ff', '80-bit',
         [0, 1, 8, 63, 64, 65, 72, 79, 80, 81, 88, 128, 129, 136]),
        ('020100000000000000001011121314151617', '138-bit',
         [0, 1, 8, 9, 16, 72, 73, 136, 137, 138, 144]),
    ]

    def __init__(self, input_hex: str, descr: str, count: int) -> None:
        self.input_hex = input_hex
        self.number_description = descr
        self.shift_count = count
        self.result = bignum_common.hex_to_int(input_hex) >> count

    def arguments(self) -> List[str]:
        return ['"{}"'.format(self.input_hex),
                str(self.shift_count),
                '"{:0{}x}"'.format(self.result, len(self.input_hex))]

    def description(self) -> str:
        return 'Core shift {} >> {}'.format(self.number_description,
                                            self.shift_count)

    @classmethod
    def generate_function_tests(cls) -> Iterator[test_case.TestCase]:
        for input_hex, descr, counts in cls.DATA:
            for count in counts:
                yield cls(input_hex, descr, count).create_test_case()


class BignumCoreShiftL(BignumCoreTarget, bignum_common.ModOperationCommon):
    """Test cases for mbedtls_bignum_core_shift_l()."""

    BIT_SHIFT_VALUES = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a',
                        '1f', '20', '21', '3f', '40', '41', '47', '48', '4f',
                        '50', '51', '58', '80', '81', '88']
    DATA = ["0", "1", "40", "dee5ca1a7ef10a75", "a1055eb0bb1efa1150ff",
            "002e7ab0070ad57001", "020100000000000000001011121314151617",
            "1946e2958a85d8863ae21f4904fcc49478412534ed53eaf321f63f2a222" +
            "7a3c63acbf50b6305595f90cfa8327f6db80d986fe96080bcbb5df1bdbe" +
            "9b74fb8dedf2bddb3f8215b54dffd66409323bcc473e45a8fe9d08e77a51" +
            "1698b5dad0416305db7fcf"]
    arity = 1
    test_function = "mpi_core_shift_l"
    test_name = "Core shift(L)"
    input_style = "arch_split"
    symbol = "<<"
    input_values = BIT_SHIFT_VALUES
    moduli = DATA

    @property
    def val_n_max_limbs(self) -> int:
        """ Return the limb count required to store the maximum number that can
        fit in a the number of digits used by val_n """
        m = bignum_common.hex_digits_max_int(self.val_n, self.bits_in_limb) - 1
        return bignum_common.limbs_mpi(m, self.bits_in_limb)

    def arguments(self) -> List[str]:
        return [bignum_common.quote_str(self.val_n),
                str(self.int_a)
                ] + self.result()

    def description(self) -> str:
        """ Format the output as:
        #{count} {hex input} ({input bits} {limbs capacity}) << {bit shift} """
        bits = "({} bits in {} limbs)".format(self.int_n.bit_length(), self.val_n_max_limbs)
        return "{} #{} {} {} {} {}".format(self.test_name,
                                           self.count,
                                           self.val_n,
                                           bits,
                                           self.symbol,
                                           self.int_a)

    def format_result(self, res: int) -> str:
        # Override to match zero-pading for leading digits between the output and input.
        res_str = bignum_common.zfill_match(self.val_n, "{:x}".format(res))
        return bignum_common.quote_str(res_str)

    def result(self) -> List[str]:
        result = (self.int_n << self.int_a)
        # Calculate if there is space for shifting to the left(leading zero limbs)
        mx = bignum_common.hex_digits_max_int(self.val_n, self.bits_in_limb)
        # If there are empty limbs ahead, adjust the bitmask accordingly
        result = result & (mx - 1)
        return [self.format_result(result)]

    @property
    def is_valid(self) -> bool:
        return True


class BignumCoreCTLookup(BignumCoreTarget, test_data_generation.BaseTest):
    """Test cases for mbedtls_mpi_core_ct_uint_table_lookup()."""
    test_function = "mpi_core_ct_uint_table_lookup"
    test_name = "Constant time MPI table lookup"

    bitsizes = [
        (32, "One limb"),
        (192, "Smallest curve sized"),
        (512, "Largest curve sized"),
        (2048, "Small FF/RSA sized"),
        (4096, "Large FF/RSA sized"),
        ]

    window_sizes = [0, 1, 2, 3, 4, 5, 6]

    def __init__(self,
                 bitsize: int, descr: str, window_size: int) -> None:
        self.bitsize = bitsize
        self.bitsize_description = descr
        self.window_size = window_size

    def arguments(self) -> List[str]:
        return [str(self.bitsize), str(self.window_size)]

    def description(self) -> str:
        return '{} - {} MPI with {} bit window'.format(
            BignumCoreCTLookup.test_name,
            self.bitsize_description,
            self.window_size
            )

    @classmethod
    def generate_function_tests(cls) -> Iterator[test_case.TestCase]:
        for bitsize, bitsize_description in cls.bitsizes:
            for window_size in cls.window_sizes:
                yield (cls(bitsize, bitsize_description, window_size)
                       .create_test_case())


class BignumCoreAddAndAddIf(BignumCoreTarget, bignum_common.OperationCommon):
    """Test cases for bignum core add and add-if."""
    count = 0
    symbol = "+"
    test_function = "mpi_core_add_and_add_if"
    test_name = "mpi_core_add_and_add_if"
    input_style = "arch_split"
    input_values = bignum_data.ADD_SUB_DATA
    unique_combinations_only = True

    def result(self) -> List[str]:
        result = self.int_a + self.int_b

        carry, result = divmod(result, self.limb_boundary)

        return [
            self.format_result(result),
            str(carry)
        ]


class BignumCoreSub(BignumCoreTarget, bignum_common.OperationCommon):
    """Test cases for bignum core sub."""
    count = 0
    input_style = "arch_split"
    symbol = "-"
    test_function = "mpi_core_sub"
    test_name = "mbedtls_mpi_core_sub"
    input_values = bignum_data.ADD_SUB_DATA

    def result(self) -> List[str]:
        if self.int_a >= self.int_b:
            result = self.int_a - self.int_b
            carry = 0
        else:
            result = self.limb_boundary + self.int_a - self.int_b
            carry = 1
        return [
            self.format_result(result),
            str(carry)
        ]


class BignumCoreMLA(BignumCoreTarget, bignum_common.OperationCommon):
    """Test cases for fixed-size multiply accumulate."""
    count = 0
    test_function = "mpi_core_mla"
    test_name = "mbedtls_mpi_core_mla"

    input_values = [
        "0", "1", "fffe", "ffffffff", "100000000", "20000000000000",
        "ffffffffffffffff", "10000000000000000", "1234567890abcdef0",
        "fffffffffffffffffefefefefefefefe",
        "100000000000000000000000000000000",
        "1234567890abcdef01234567890abcdef0",
        "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff",
        "1234567890abcdef01234567890abcdef01234567890abcdef01234567890abcdef0",
        (
            "4df72d07b4b71c8dacb6cffa954f8d88254b6277099308baf003fab73227f" +
            "34029643b5a263f66e0d3c3fa297ef71755efd53b8fb6cb812c6bbf7bcf17" +
            "9298bd9947c4c8b14324140a2c0f5fad7958a69050a987a6096e9f055fb38" +
            "edf0c5889eca4a0cfa99b45fbdeee4c696b328ddceae4723945901ec02507" +
            "6b12b"
        )
    ] # type: List[str]
    input_scalars = [
        "0", "3", "fe", "ff", "ffff", "10000", "ffffffff", "100000000",
        "7f7f7f7f7f7f7f7f", "8000000000000000", "fffffffffffffffe"
    ] # type: List[str]

    def __init__(self, val_a: str, val_b: str, val_s: str) -> None:
        super().__init__(val_a, val_b)
        self.arg_scalar = val_s
        self.int_scalar = bignum_common.hex_to_int(val_s)
        if bignum_common.limbs_mpi(self.int_scalar, 32) > 1:
            self.dependencies = ["MBEDTLS_HAVE_INT64"]

    def arguments(self) -> List[str]:
        return [
            bignum_common.quote_str(self.arg_a),
            bignum_common.quote_str(self.arg_b),
            bignum_common.quote_str(self.arg_scalar)
        ] + self.result()

    def description(self) -> str:
        """Override and add the additional scalar."""
        if not self.case_description:
            self.case_description = "0x{} + 0x{} * 0x{}".format(
                self.arg_a, self.arg_b, self.arg_scalar
            )
        return super().description()

    def result(self) -> List[str]:
        result = self.int_a + (self.int_b * self.int_scalar)
        bound_val = max(self.int_a, self.int_b)
        bound_4 = bignum_common.bound_mpi(bound_val, 32)
        bound_8 = bignum_common.bound_mpi(bound_val, 64)
        carry_4, remainder_4 = divmod(result, bound_4)
        carry_8, remainder_8 = divmod(result, bound_8)
        return [
            "\"{:x}\"".format(remainder_4),
            "\"{:x}\"".format(carry_4),
            "\"{:x}\"".format(remainder_8),
            "\"{:x}\"".format(carry_8)
        ]

    @classmethod
    def get_value_pairs(cls) -> Iterator[Tuple[str, str]]:
        """Generator to yield pairs of inputs.

        Combinations are first generated from all input values, and then
        specific cases provided.
        """
        yield from super().get_value_pairs()
        yield from cls.input_cases

    @classmethod
    def generate_function_tests(cls) -> Iterator[test_case.TestCase]:
        """Override for additional scalar input."""
        for a_value, b_value in cls.get_value_pairs():
            for s_value in cls.input_scalars:
                cur_op = cls(a_value, b_value, s_value)
                yield cur_op.create_test_case()


class BignumCoreMul(BignumCoreTarget, bignum_common.OperationCommon):
    """Test cases for bignum core multiplication."""
    count = 0
    input_style = "arch_split"
    symbol = "*"
    test_function = "mpi_core_mul"
    test_name = "mbedtls_mpi_core_mul"
    arity = 2
    unique_combinations_only = True

    def format_arg(self, val: str) -> str:
        return val

    def format_result(self, res: int) -> str:
        res_str = '{:x}'.format(res)
        a_limbs = bignum_common.limbs_mpi(self.int_a, self.bits_in_limb)
        b_limbs = bignum_common.limbs_mpi(self.int_b, self.bits_in_limb)
        hex_digits = bignum_common.hex_digits_for_limb(a_limbs + b_limbs, self.bits_in_limb)
        return bignum_common.quote_str(self.format_arg(res_str).zfill(hex_digits))

    def result(self) -> List[str]:
        result = self.int_a * self.int_b
        return [self.format_result(result)]


class BignumCoreMontmul(BignumCoreTarget, test_data_generation.BaseTest):
    """Test cases for Montgomery multiplication."""
    count = 0
    test_function = "mpi_core_montmul"
    test_name = "mbedtls_mpi_core_montmul"

    start_2_mpi4 = False
    start_2_mpi8 = False

    replay_test_cases = [
        (2, 1, 1, 1, "19", "1", "1D"), (2, 1, 1, 1, "7", "1", "9"),
        (2, 1, 1, 1, "4", "1", "9"),
        (
            12, 1, 6, 1, (
                "3C246D0E059A93A266288A7718419EC741661B474C58C032C5EDAF92709402" +
                "B07CC8C7CE0B781C641A1EA8DB2F4343"
            ), "1", (
                "66A198186C18C10B2F5ED9B522752A9830B69916E535C8F047518A889A43A5" +
                "94B6BED27A168D31D4A52F88925AA8F5"
            )
        ), (
            8, 1, 4, 1,
            "1E442976B0E63D64FCCE74B999E470CA9888165CB75BFA1F340E918CE03C6211",
            "1", "B3A119602EE213CDE28581ECD892E0F592A338655DCE4CA88054B3D124D0E561"
        ), (
            22, 1, 11, 1, (
                "7CF5AC97304E0B63C65413F57249F59994B0FED1D2A8D3D83ED5FA38560FFB" +
                "82392870D6D08F87D711917FD7537E13B7E125BE407E74157776839B0AC9DB" +
                "23CBDFC696104353E4D2780B2B4968F8D8542306BCA7A2366E"
            ), "1", (
                "284139EA19C139EBE09A8111926AAA39A2C2BE12ED487A809D3CB5BC558547" +
                "25B4CDCB5734C58F90B2F60D99CC1950CDBC8D651793E93C9C6F0EAD752500" +
                "A32C56C62082912B66132B2A6AA42ADA923E1AD22CEB7BA0123"
            )
        )
    ] # type: List[Tuple[int, int, int, int, str, str, str]]

    random_test_cases = [
        ("2", "2", "3", ""), ("1", "2", "3", ""), ("2", "1", "3", ""),
        ("6", "5", "7", ""), ("3", "4", "7", ""), ("1", "6", "7", ""), ("5", "6", "7", ""),
        ("3", "4", "B", ""), ("7", "4", "B", ""), ("9", "7", "B", ""), ("2", "a", "B", ""),
        ("25", "16", "29", "(0x29 is prime)"), ("8", "28", "29", ""),
        ("18", "21", "29", ""), ("15", "f", "29", ""),
        ("e2", "ea", "FF", ""), ("43", "72", "FF", ""),
        ("d8", "70", "FF", ""), ("3c", "7c", "FF", ""),
        ("99", "b9", "101", "(0x101 is prime)"), ("65", "b2", "101", ""),
        ("81", "32", "101", ""), ("51", "dd", "101", ""),
        ("d5", "143", "38B", "(0x38B is prime)"), ("3d", "387", "38B", ""),
        ("160", "2e5", "38B", ""), ("10f", "137", "38B", ""),
        ("7dac", "25a", "8003", "(0x8003 is prime)"), ("6f1c", "3286", "8003", ""),
        ("59ed", "2f3f", "8003", ""), ("6893", "736d", "8003", ""),
        ("d199", "2832", "10001", "(0x10001 is prime)"), ("c3b2", "3e5b", "10001", ""),
        ("abe4", "214e", "10001", ""), ("4360", "a05d", "10001", ""),
        ("3f5a1", "165b2", "7F7F7", ""), ("3bd29", "37863", "7F7F7", ""),
        ("60c47", "64819", "7F7F7", ""), ("16584", "12c49", "7F7F7", ""),
        ("1ff03f", "610347", "800009", "(0x800009 is prime)"), ("340fd5", "19812e", "800009", ""),
        ("3fe2e8", "4d0dc7", "800009", ""), ("40356", "e6392", "800009", ""),
        ("dd8a1d", "266c0e", "100002B", "(0x100002B is prime)"),
        ("3fa1cb", "847fd6", "100002B", ""), ("5f439d", "5c3196", "100002B", ""),
        ("18d645", "f72dc6", "100002B", ""),
        ("20051ad", "37def6e", "37EEE9D", "(0x37EEE9D is prime)"),
        ("2ec140b", "3580dbf", "37EEE9D", ""), ("1d91b46", "190d4fc", "37EEE9D", ""),
        ("34e488d", "1224d24", "37EEE9D", ""),
        ("2a4fe2cb", "263466a9", "8000000B", "(0x8000000B is prime)"),
        ("5643fe94", "29a1aefa", "8000000B", ""), ("29633513", "7b007ac4", "8000000B", ""),
        ("2439cef5", "5c9d5a47", "8000000B", ""),
        ("4de3cfaa", "50dea178", "8CD626B9", "(0x8CD626B9 is prime)"),
        ("b8b8563", "10dbbbac", "8CD626B9", ""), ("4e8a6151", "5574ec19", "8CD626B9", ""),
        ("69224878", "309cfc23", "8CD626B9", ""),
        ("fb6f7fb6", "afb05423", "10000000F", "(0x10000000F is prime)"),
        ("8391a243", "26034dcd", "10000000F", ""), ("d26b98c", "14b2d6aa", "10000000F", ""),
        ("6b9f1371", "a21daf1d", "10000000F", ""),
        (
            "9f49435ad", "c8264ade8", "174876E7E9",
            "0x174876E7E9 is prime (dec) 99999999977"
        ),
        ("c402da434", "1fb427acf", "174876E7E9", ""),
        ("f6ebc2bb1", "1096d39f2a", "174876E7E9", ""),
        ("153b7f7b6b", "878fda8ff", "174876E7E9", ""),
        ("2c1adbb8d6", "4384d2d3c6", "8000000017", "(0x8000000017 is prime)"),
        ("2e4f9cf5fb", "794f3443d9", "8000000017", ""),
        ("149e495582", "3802b8f7b7", "8000000017", ""),
        ("7b9d49df82", "69c68a442a", "8000000017", ""),
        ("683a134600", "6dd80ea9f6", "864CB9076D", "(0x864CB9076D is prime)"),
        ("13a870ff0d", "59b099694a", "864CB9076D", ""),
        ("37d06b0e63", "4d2147e46f", "864CB9076D", ""),
        ("661714f8f4", "22e55df507", "864CB9076D", ""),
        ("2f0a96363", "52693307b4", "F7F7F7F7F7", ""),
        ("3c85078e64", "f2275ecb6d", "F7F7F7F7F7", ""),
        ("352dae68d1", "707775b4c6", "F7F7F7F7F7", ""),
        ("37ae0f3e0b", "912113040f", "F7F7F7F7F7", ""),
        ("6dada15e31", "f58ed9eff7", "1000000000F", "(0x1000000000F is prime)"),
        ("69627a7c89", "cfb5ebd13d", "1000000000F", ""),
        ("a5e1ad239b", "afc030c731", "1000000000F", ""),
        ("f1cc45f4c5", "c64ad607c8", "1000000000F", ""),
        ("2ebad87d2e31", "4c72d90bca78", "800000000005", "(0x800000000005 is prime)"),
        ("a30b3cc50d", "29ac4fe59490", "800000000005", ""),
        ("33674e9647b4", "5ec7ee7e72d3", "800000000005", ""),
        ("3d956f474f61", "74070040257d", "800000000005", ""),
        ("48348e3717d6", "43fcb4399571", "800795D9BA47", "(0x800795D9BA47 is prime)"),
        ("5234c03cc99b", "2f3cccb87803", "800795D9BA47", ""),
        ("3ed13db194ab", "44b8f4ba7030", "800795D9BA47", ""),
        ("1c11e843bfdb", "95bd1b47b08", "800795D9BA47", ""),
        ("a81d11cb81fd", "1e5753a3f33d", "1000000000015", "(0x1000000000015 is prime)"),
        ("688c4db99232", "36fc0cf7ed", "1000000000015", ""),
        ("f0720cc07e07", "fc76140ed903", "1000000000015", ""),
        ("2ec61f8d17d1", "d270c85e36d2", "1000000000015", ""),
        (
            "6a24cd3ab63820", "ed4aad55e5e348", "100000000000051",
            "(0x100000000000051 is prime)"
        ),
        ("e680c160d3b248", "31e0d8840ed510", "100000000000051", ""),
        ("a80637e9aebc38", "bb81decc4e1738", "100000000000051", ""),
        ("9afa5a59e9d630", "be9e65a6d42938", "100000000000051", ""),
        ("ab5e104eeb71c000", "2cffbd639e9fea00", "ABCDEF0123456789", ""),
        ("197b867547f68a00", "44b796cf94654800", "ABCDEF0123456789", ""),
        ("329f9483a04f2c00", "9892f76961d0f000", "ABCDEF0123456789", ""),
        ("4a2e12dfb4545000", "1aa3e89a69794500", "ABCDEF0123456789", ""),
        (
            "8b9acdf013d140f000", "12e4ceaefabdf2b2f00", "25A55A46E5DA99C71C7",
            "0x25A55A46E5DA99C71C7 is the 3rd repunit prime(dec) 11111111111111111111111"
        ),
        ("1b8d960ea277e3f5500", "14418aa980e37dd000", "25A55A46E5DA99C71C7", ""),
        ("7314524977e8075980", "8172fa45618ccd0d80", "25A55A46E5DA99C71C7", ""),
        ("ca14f031769be63580", "147a2f3cf2964ca9400", "25A55A46E5DA99C71C7", ""),
        (
            "18532ba119d5cd0cf39735c0000", "25f9838e31634844924733000000",
            "314DC643FB763F2B8C0E2DE00879",
            "0x314DC643FB763F2B8C0E2DE00879 is (dec)99999999977^3"
        ),
        (
            "a56e2d2517519e3970e70c40000", "ec27428d4bb380458588fa80000",
            "314DC643FB763F2B8C0E2DE00879", ""
        ),
        (
            "1cb5e8257710e8653fff33a00000", "15fdd42fe440fd3a1d121380000",
            "314DC643FB763F2B8C0E2DE00879", ""
        ),
        (
            "e50d07a65fc6f93e538ce040000", "1f4b059ca609f3ce597f61240000",
            "314DC643FB763F2B8C0E2DE00879", ""
        ),
        (
            "1ea3ade786a095d978d387f30df9f20000000",
            "127c448575f04af5a367a7be06c7da0000000",
            "47BF19662275FA2F6845C74942ED1D852E521",
            "0x47BF19662275FA2F6845C74942ED1D852E521 is (dec) 99999999977^4"
        ),
        (
            "16e15b0ca82764e72e38357b1f10a20000000",
            "43e2355d8514bbe22b0838fdc3983a0000000",
            "47BF19662275FA2F6845C74942ED1D852E521", ""
        ),
        (
            "be39332529d93f25c3d116c004c620000000",
            "5cccec42370a0a2c89c6772da801a0000000",
            "47BF19662275FA2F6845C74942ED1D852E521", ""
        ),
        (
            "ecaa468d90de0eeda474d39b3e1fc0000000",
            "1e714554018de6dc0fe576bfd3b5660000000",
            "47BF19662275FA2F6845C74942ED1D852E521", ""
        ),
        (
            "32298816711c5dce46f9ba06e775c4bedfc770e6700000000000000",
            "8ee751fd5fb24f0b4a653cb3a0c8b7d9e724574d168000000000000",
            "97EDD86E4B5C4592C6D32064AC55C888A7245F07CA3CC455E07C931",
            (
                "0x97EDD86E4B5C4592C6D32064AC55C888A7245F07CA3CC455E07C931" +
                " is (dec) 99999999977^6"
            )
        ),
        (
            "29213b9df3cfd15f4b428645b67b677c29d1378d810000000000000",
            "6cbb732c65e10a28872394dfdd1936d5171c3c3aac0000000000000",
            "97EDD86E4B5C4592C6D32064AC55C888A7245F07CA3CC455E07C931", ""
        ),
        (
            "6f18db06ad4abc52c0c50643dd13098abccd4a232f0000000000000",
            "7e6bf41f2a86098ad51f98dfc10490ba3e8081bc830000000000000",
            "97EDD86E4B5C4592C6D32064AC55C888A7245F07CA3CC455E07C931", ""
        ),
        (
            "62d3286cd706ad9d73caff63f1722775d7e8c731208000000000000",
            "530f7ba02ae2b04c2fe3e3d27ec095925631a6c2528000000000000",
            "97EDD86E4B5C4592C6D32064AC55C888A7245F07CA3CC455E07C931", ""
        ),
        (
            "a6c6503e3c031fdbf6009a89ed60582b7233c5a85de28b16000000000000000",
            "75c8ed18270b583f16d442a467d32bf95c5e491e9b8523798000000000000000",
            "DD15FE80B731872AC104DB37832F7E75A244AA2631BC87885B861E8F20375499",
            (
                "0xDD15FE80B731872AC104DB37832F7E75A244AA2631BC87885B861E8F20375499" +
                " is (dec) 99999999977^7"
            )
        ),
        (
            "bf84d1f85cf6b51e04d2c8f4ffd03532d852053cf99b387d4000000000000000",
            "397ba5a743c349f4f28bc583ecd5f06e0a25f9c6d98f09134000000000000000",
            "DD15FE80B731872AC104DB37832F7E75A244AA2631BC87885B861E8F20375499", ""
        ),
        (
            "6db11c3a4152ed1a2aa6fa34b0903ec82ea1b88908dcb482000000000000000",
            "ac8ac576a74ad6ca48f201bf89f77350ce86e821358d85920000000000000000",
            "DD15FE80B731872AC104DB37832F7E75A244AA2631BC87885B861E8F20375499", ""
        ),
        (
            "3001d96d7fe8b733f33687646fc3017e3ac417eb32e0ec708000000000000000",
            "925ddbdac4174e8321a48a32f79640e8cf7ec6f46ea235a80000000000000000",
            "DD15FE80B731872AC104DB37832F7E75A244AA2631BC87885B861E8F20375499", ""
        ),
        (
            "1029048755f2e60dd98c8de6d9989226b6bb4f0db8e46bd1939de560000000000000000000",
            "51bb7270b2e25cec0301a03e8275213bb6c2f6e6ec93d4d46d36ca0000000000000000000",
            "141B8EBD9009F84C241879A1F680FACCED355DA36C498F73E96E880CF78EA5F96146380E41",
            (
                "0x141B8EBD9009F84C241879A1F680FACCED355DA36C498F73E96E880CF78EA5F96146" +
                "380E41 is 99999999977^8"
            )
        ),
        (
            "1c5337ff982b3ad6611257dbff5bbd7a9920ba2d4f5838a0cc681ce000000000000000000",
            "520c5d049ca4702031ba728591b665c4d4ccd3b2b86864d4c160fd2000000000000000000",
            "141B8EBD9009F84C241879A1F680FACCED355DA36C498F73E96E880CF78EA5F96146380E41",
            ""
        ),
        (
            "57074dfa00e42f6555bae624b7f0209f218adf57f73ed34ab0ff90c000000000000000000",
            "41eb14b6c07bfd3d1fe4f4a610c17cc44fcfcda695db040e011065000000000000000000",
            "141B8EBD9009F84C241879A1F680FACCED355DA36C498F73E96E880CF78EA5F96146380E41",
            ""
        ),
        (
            "d8ed7feed2fe855e6997ad6397f776158573d425031bf085a615784000000000000000000",
            "6f121dcd18c578ab5e229881006007bb6d319b179f11015fe958b9c000000000000000000",
            "141B8EBD9009F84C241879A1F680FACCED355DA36C498F73E96E880CF78EA5F96146380E41",
            ""
        ),
        (
            (
                "2a462b156180ea5fe550d3758c764e06fae54e626b5f503265a09df76edbdfbf" +
                "a1e6000000000000000000000000"
            ), (
                "1136f41d1879fd4fb9e49e0943a46b6704d77c068ee237c3121f9071cfd3e6a0" +
                "0315800000000000000000000000"
            ), (
                "2A94608DE88B6D5E9F8920F5ABB06B24CC35AE1FBACC87D075C621C3E2833EC90" +
                "2713E40F51E3B3C214EDFABC451"
            ), (
                "0x2A94608DE88B6D5E9F8920F5ABB06B24CC35AE1FBACC87D075C621C3E2833EC" +
                "902713E40F51E3B3C214EDFABC451 is (dec) 99999999977^10"
            )
        ),
        (
            (
                "c1ac3800dfb3c6954dea391d206200cf3c47f795bf4a5603b4cb88ae7e574de47" +
                "40800000000000000000000000"
            ), (
                "c0d16eda0549ede42fa0deb4635f7b7ce061fadea02ee4d85cba4c4f709603419" +
                "3c800000000000000000000000"
            ), (
                "2A94608DE88B6D5E9F8920F5ABB06B24CC35AE1FBACC87D075C621C3E2833EC90" +
                "2713E40F51E3B3C214EDFABC451"
            ), ""
        ),
        (
            (
                "19e45bb7633094d272588ad2e43bcb3ee341991c6731b6fa9d47c4018d7ce7bba" +
                "5ee800000000000000000000000"
            ), (
                "1e4f83166ae59f6b9cc8fd3e7677ed8bfc01bb99c98bd3eb084246b64c1e18c33" +
                "65b800000000000000000000000"
            ), (
                "2A94608DE88B6D5E9F8920F5ABB06B24CC35AE1FBACC87D075C621C3E2833EC90" +
                "2713E40F51E3B3C214EDFABC451"
            ), ""
        ),
        (
            (
                "1aa93395fad5f9b7f20b8f9028a054c0bb7c11bb8520e6a95e5a34f06cb70bcdd" +
                "01a800000000000000000000000"
            ), (
                "54b45afa5d4310192f8d224634242dd7dcfb342318df3d9bd37b4c614788ba13b" +
                "8b000000000000000000000000"
            ), (
                "2A94608DE88B6D5E9F8920F5ABB06B24CC35AE1FBACC87D075C621C3E2833EC90" +
                "2713E40F51E3B3C214EDFABC451"
            ), ""
        ),
        (
            (
                "544f2628a28cfb5ce0a1b7180ee66b49716f1d9476c466c57f0c4b23089917843" +
                "06d48f78686115ee19e25400000000000000000000000000000000"
            ), (
                "677eb31ef8d66c120fa872a60cd47f6e10cbfdf94f90501bd7883cba03d185be0" +
                "a0148d1625745e9c4c827300000000000000000000000000000000"
            ), (
                "8335616AED761F1F7F44E6BD49E807B82E3BF2BF11BFA6AF813C808DBF33DBFA1" +
                "1DABD6E6144BEF37C6800000000000000000000000000000000051"
            ), (
                "0x8335616AED761F1F7F44E6BD49E807B82E3BF2BF11BFA6AF813C808DBF33DBF" +
                "A11DABD6E6144BEF37C6800000000000000000000000000000000051 is prime," +
                " (dec) 10^143 + 3^4"
            )
        ),
        (
            (
                "76bb3470985174915e9993522aec989666908f9e8cf5cb9f037bf4aee33d8865c" +
                "b6464174795d07e30015b80000000000000000000000000000000"
            ), (
                "6aaaf60d5784dcef612d133613b179a317532ecca0eed40b8ad0c01e6d4a6d8c7" +
                "9a52af190abd51739009a900000000000000000000000000000000"
            ), (
                "8335616AED761F1F7F44E6BD49E807B82E3BF2BF11BFA6AF813C808DBF33DBFA1" +
                "1DABD6E6144BEF37C6800000000000000000000000000000000051"
            ), ""
        ),
        (
            (
                "6cfdd6e60912e441d2d1fc88f421b533f0103a5322ccd3f4db84861643ad63fd6" +
                "3d1d8cfbc1d498162786ba00000000000000000000000000000000"
            ), (
                "1177246ec5e93814816465e7f8f248b350d954439d35b2b5d75d917218e7fd5fb" +
                "4c2f6d0667f9467fdcf33400000000000000000000000000000000"
            ), (
                "8335616AED761F1F7F44E6BD49E807B82E3BF2BF11BFA6AF813C808DBF33DBFA1" +
                "1DABD6E6144BEF37C6800000000000000000000000000000000051"
            ), ""
        ),
        (
            (
                "7a09a0b0f8bbf8057116fb0277a9bdf3a91b5eaa8830d448081510d8973888be5" +
                "a9f0ad04facb69aa3715f00000000000000000000000000000000"
            ), (
                "764dec6c05a1c0d87b649efa5fd94c91ea28bffb4725d4ab4b33f1a3e8e3b314d" +
                "799020e244a835a145ec9800000000000000000000000000000000"
            ), (
                "8335616AED761F1F7F44E6BD49E807B82E3BF2BF11BFA6AF813C808DBF33DBFA1" +
                "1DABD6E6144BEF37C6800000000000000000000000000000000051"
            ), ""
        )
    ] # type: List[Tuple[str, str, str, str]]

    def __init__(
            self, val_a: str, val_b: str, val_n: str, case_description: str = ""
        ):
        self.case_description = case_description
        self.arg_a = val_a
        self.int_a = bignum_common.hex_to_int(val_a)
        self.arg_b = val_b
        self.int_b = bignum_common.hex_to_int(val_b)
        self.arg_n = val_n
        self.int_n = bignum_common.hex_to_int(val_n)

        limbs_a4 = bignum_common.limbs_mpi(self.int_a, 32)
        limbs_a8 = bignum_common.limbs_mpi(self.int_a, 64)
        self.limbs_b4 = bignum_common.limbs_mpi(self.int_b, 32)
        self.limbs_b8 = bignum_common.limbs_mpi(self.int_b, 64)
        self.limbs_an4 = bignum_common.limbs_mpi(self.int_n, 32)
        self.limbs_an8 = bignum_common.limbs_mpi(self.int_n, 64)

        if limbs_a4 > self.limbs_an4 or limbs_a8 > self.limbs_an8:
            raise Exception("Limbs of input A ({}) exceeds N ({})".format(
                self.arg_a, self.arg_n
            ))

    def arguments(self) -> List[str]:
        return [
            str(self.limbs_an4), str(self.limbs_b4),
            str(self.limbs_an8), str(self.limbs_b8),
            bignum_common.quote_str(self.arg_a),
            bignum_common.quote_str(self.arg_b),
            bignum_common.quote_str(self.arg_n)
        ] + self.result()

    def description(self) -> str:
        if self.case_description != "replay":
            if not self.start_2_mpi4 and self.limbs_an4 > 1:
                tmp = "(start of 2-MPI 4-byte bignums) "
                self.__class__.start_2_mpi4 = True
            elif not self.start_2_mpi8 and self.limbs_an8 > 1:
                tmp = "(start of 2-MPI 8-byte bignums) "
                self.__class__.start_2_mpi8 = True
            else:
                tmp = "(gen) "
            self.case_description = tmp + self.case_description
        return super().description()

    def result(self) -> List[str]:
        """Get the result of the operation."""
        r4 = bignum_common.bound_mpi_limbs(self.limbs_an4, 32)
        i4 = bignum_common.invmod(r4, self.int_n)
        x4 = self.int_a * self.int_b * i4
        x4 = x4 % self.int_n

        r8 = bignum_common.bound_mpi_limbs(self.limbs_an8, 64)
        i8 = bignum_common.invmod(r8, self.int_n)
        x8 = self.int_a * self.int_b * i8
        x8 = x8 % self.int_n
        return [
            "\"{:x}\"".format(x4),
            "\"{:x}\"".format(x8)
        ]

    def set_limbs(
            self, limbs_an4: int, limbs_b4: int, limbs_an8: int, limbs_b8: int
        ) -> None:
        """Set number of limbs for each input.

        Replaces default values set during initialization.
        """
        self.limbs_an4 = limbs_an4
        self.limbs_b4 = limbs_b4
        self.limbs_an8 = limbs_an8
        self.limbs_b8 = limbs_b8

    @classmethod
    def generate_function_tests(cls) -> Iterator[test_case.TestCase]:
        """Generate replay and randomly generated test cases."""
        # Test cases which replay captured invocations during unit test runs.
        for limbs_an4, limbs_b4, limbs_an8, limbs_b8, a, b, n in cls.replay_test_cases:
            cur_op = cls(a, b, n, case_description="replay")
            cur_op.set_limbs(limbs_an4, limbs_b4, limbs_an8, limbs_b8)
            yield cur_op.create_test_case()
        # Random test cases can be generated using mpi_modmul_case_generate()
        # Uses a mixture of primes and odd numbers as N, with four randomly
        # generated cases for each N.
        for a, b, n, description in cls.random_test_cases:
            cur_op = cls(a, b, n, case_description=description)
            yield cur_op.create_test_case()


def mpi_modmul_case_generate() -> None:
    """Generate valid inputs for montmul tests using moduli.

    For each modulus, generates random values for A and B and simple descriptions
    for the test case.
    """
    moduli = [
        ("3", ""), ("7", ""), ("B", ""), ("29", ""), ("FF", ""),
        ("101", ""), ("38B", ""), ("8003", ""), ("10001", ""),
        ("7F7F7", ""), ("800009", ""), ("100002B", ""), ("37EEE9D", ""),
        ("8000000B", ""), ("8CD626B9", ""), ("10000000F", ""),
        ("174876E7E9", "is prime (dec) 99999999977"),
        ("8000000017", ""), ("864CB9076D", ""), ("F7F7F7F7F7", ""),
        ("1000000000F", ""), ("800000000005", ""), ("800795D9BA47", ""),
        ("1000000000015", ""), ("100000000000051", ""), ("ABCDEF0123456789", ""),
        (
            "25A55A46E5DA99C71C7",
            "is the 3rd repunit prime (dec) 11111111111111111111111"
        ),
        ("314DC643FB763F2B8C0E2DE00879", "is (dec)99999999977^3"),
        ("47BF19662275FA2F6845C74942ED1D852E521", "is (dec) 99999999977^4"),
        (
            "97EDD86E4B5C4592C6D32064AC55C888A7245F07CA3CC455E07C931",
            "is (dec) 99999999977^6"
        ),
        (
            "DD15FE80B731872AC104DB37832F7E75A244AA2631BC87885B861E8F20375499",
            "is (dec) 99999999977^7"
        ),
        (
            "141B8EBD9009F84C241879A1F680FACCED355DA36C498F73E96E880CF78EA5F96146380E41",
            "is (dec) 99999999977^8"
        ),
        (
            (
                "2A94608DE88B6D5E9F8920F5ABB06B24CC35AE1FBACC87D075C621C3E283" +
                "3EC902713E40F51E3B3C214EDFABC451"
            ),
            "is (dec) 99999999977^10"
        ),
        (
            "8335616AED761F1F7F44E6BD49E807B82E3BF2BF11BFA6AF813C808DBF33DBFA11" +
            "DABD6E6144BEF37C6800000000000000000000000000000000051",
            "is prime, (dec) 10^143 + 3^4"
        )
    ] # type: List[Tuple[str, str]]
    primes = [
        "3", "7", "B", "29", "101", "38B", "8003", "10001", "800009",
        "100002B", "37EEE9D", "8000000B", "8CD626B9",
        # From here they require > 1 4-byte MPI
        "10000000F", "174876E7E9", "8000000017", "864CB9076D", "1000000000F",
        "800000000005", "800795D9BA47", "1000000000015", "100000000000051",
        # From here they require > 1 8-byte MPI
        "25A55A46E5DA99C71C7",      # this is 11111111111111111111111 decimal
        # 10^143 + 3^4: (which is prime)
        # 100000000000000000000000000000000000000000000000000000000000000000000000000000
        # 000000000000000000000000000000000000000000000000000000000000000081
        (
            "8335616AED761F1F7F44E6BD49E807B82E3BF2BF11BFA6AF813C808DBF33DBFA11" +
            "DABD6E6144BEF37C6800000000000000000000000000000000051"
        )
    ] # type: List[str]
    generated_inputs = []
    for mod, description in moduli:
        n = bignum_common.hex_to_int(mod)
        mod_read = "{:x}".format(n)
        case_count = 3 if n < 5 else 4
        cases = {} # type: Dict[int, int]
        i = 0
        while i < case_count:
            a = random.randint(1, n)
            b = random.randint(1, n)
            if cases.get(a) == b:
                continue
            cases[a] = b
            if description:
                out_description = "0x{} {}".format(mod_read, description)
            elif i == 0 and len(mod) > 1 and mod in primes:
                out_description = "(0x{} is prime)"
            else:
                out_description = ""
            generated_inputs.append(
                ("{:x}".format(a), "{:x}".format(b), mod, out_description)
            )
            i += 1
    print(generated_inputs)


class BignumCoreExpMod(BignumCoreTarget, bignum_common.ModOperationCommon):
    """Test cases for bignum core exponentiation."""
    symbol = "^"
    test_function = "mpi_core_exp_mod"
    test_name = "Core modular exponentiation (Mongtomery form only)"
    input_style = "fixed"
    montgomery_form_a = True

    def result(self) -> List[str]:
        # Result has to be given in Montgomery form too
        result = pow(self.int_a, self.int_b, self.int_n)
        mont_result = self.to_montgomery(result)
        return [self.format_result(mont_result)]

    @property
    def is_valid(self) -> bool:
        # The base needs to be canonical, but the exponent can be larger than
        # the modulus (see for example exponent blinding)
        return bool(self.int_a < self.int_n)


class BignumCoreSubInt(BignumCoreTarget, bignum_common.OperationCommon):
    """Test cases for bignum core sub int."""
    count = 0
    symbol = "-"
    test_function = "mpi_core_sub_int"
    test_name = "mpi_core_sub_int"
    input_style = "arch_split"

    @property
    def is_valid(self) -> bool:
        # This is "sub int", so b is only one limb
        if bignum_common.limbs_mpi(self.int_b, self.bits_in_limb) > 1:
            return False
        return True

    # Overriding because we don't want leading zeros on b
    @property
    def arg_b(self) -> str:
        return self.val_b

    def result(self) -> List[str]:
        result = self.int_a - self.int_b

        borrow, result = divmod(result, self.limb_boundary)

        # Borrow will be -1 if non-zero, but we want it to be 1 in the test data
        return [
            self.format_result(result),
            str(-borrow)
        ]

class BignumCoreZeroCheckCT(BignumCoreTarget, bignum_common.OperationCommon):
    """Test cases for bignum core zero check (constant flow)."""
    count = 0
    symbol = "== 0"
    test_function = "mpi_core_check_zero_ct"
    test_name = "mpi_core_check_zero_ct"
    input_style = "variable"
    arity = 1
    suffix = True

    def result(self) -> List[str]:
        result = 1 if self.int_a == 0 else 0
        return [str(result)]

class BignumCoreGcdModinvOdd(BignumCoreTarget, test_data_generation.BaseTest):
    """Test cases for bignum core GCD+modinv (odd modulus)"""

    test_function = "mpi_core_gcd_modinv_odd"
    test_name = "mpi_core_gcd_modinv_odd"

    # - All small integers because that naturally covers a lot of cases.
    # - (Close to) powers of 2 because that looks like interesting values.
    #   Also covers the cases where N, N+1 or N-1 is a multiple of A (with
    #   multiple limbs).
    # - X * 2, X * 3 is so that we get GCD(X*2, X*3) = X where the GCD has a
    #   the same order of magnitude as the inputs.
    # - Random values of cryptographic size for good measure.
    DATA = (
        ("0", 0),
        ("1", 1),
        ("2", 2),
        ("3", 3),
        ("4", 4),
        ("5", 5),
        ("6", 6),
        ("7", 7),
        ("2^64 - 1", 2**64 - 1),
        ("2^64", 2**64),
        ("2^64 + 1", 2**64 + 1),
        ("2^128 - 1", 2**128 - 1),
        ("2^128", 2**128),
        ("2^128 + 1", 2**128 + 1),
        ("prime192[1]", int(bignum_data.SAFE_PRIME_192_BIT_SEED_1, 16)),
        ("prime192[1] * 2", int(bignum_data.SAFE_PRIME_192_BIT_SEED_1, 16) * 2),
        ("prime192[1] * 3", int(bignum_data.SAFE_PRIME_192_BIT_SEED_1, 16) * 3),
        ("rand192[2.1]", int(bignum_data.RANDOM_192_BIT_SEED_2_NO1, 16)),
        ("rand192[2.2]", int(bignum_data.RANDOM_192_BIT_SEED_2_NO2, 16)),
        ("rand192[2.3]", int(bignum_data.RANDOM_192_BIT_SEED_2_NO3, 16)),
        ("rand192[2.4]", int(bignum_data.RANDOM_192_BIT_SEED_2_NO4, 16)),
        ("rand192[2.9]", int(bignum_data.RANDOM_192_BIT_SEED_2_NO9, 16)),
        ("prime1024[3]", int(bignum_data.SAFE_PRIME_1024_BIT_SEED_3, 16)),
        ("rand1024[4.1]", int(bignum_data.RANDOM_1024_BIT_SEED_4_NO1, 16)),
        ("rand1024[4.2]", int(bignum_data.RANDOM_1024_BIT_SEED_4_NO2, 16)),
        ("rand1024[4.3]", int(bignum_data.RANDOM_1024_BIT_SEED_4_NO3, 16)),
        ("rand1024[4.4]", int(bignum_data.RANDOM_1024_BIT_SEED_4_NO4, 16)),
        ("rand1024[4.5]", int(bignum_data.RANDOM_1024_BIT_SEED_4_NO5, 16)),
        ("rand1024[4.5] * 2", int(bignum_data.RANDOM_1024_BIT_SEED_4_NO5, 16) * 2),
        ("rand1024[4.5] * 3", int(bignum_data.RANDOM_1024_BIT_SEED_4_NO5, 16) * 3),
    )

    def __init__(self, a: int, a_desc: str, n: int, n_desc: str) -> None:
        self.a_val = a
        self.a_desc = a_desc
        self.n_val = n
        self.n_desc = n_desc
        self.g_val = math.gcd(a, n)
        test_i = self.g_val == 1 and self.n_val != 1
        self.i_val = bignum_common.invmod_positive(a, n) if test_i else None

    def arguments(self) -> List[str]:
        a_str = f"{self.a_val:x}"
        n_str = f"{self.n_val:x}"
        g_str = f"{self.g_val:x}"
        i_str = f"{self.i_val:x}" if self.i_val is not None else ""
        return [bignum_common.quote_str(s) for s in (a_str, n_str, g_str, i_str)]

    def description(self) -> str:
        return f"GCD-modinv, A = {self.a_desc}, N = {self.n_desc}"

    @classmethod
    def generate_function_tests(cls) -> Iterator[test_case.TestCase]:
        for n_desc, n in cls.DATA:
            if n % 2 == 0:
                continue
            for a_desc, a in cls.DATA:
                if a > n:
                    continue
                yield cls(a, a_desc, n, n_desc).create_test_case()
