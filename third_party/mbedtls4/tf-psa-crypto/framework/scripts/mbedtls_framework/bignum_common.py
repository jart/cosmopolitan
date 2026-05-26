"""Common features for bignum in test generation framework."""
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#

from abc import abstractmethod
import enum
from typing import Iterator, List, Tuple, TypeVar, Any
from copy import deepcopy
from itertools import chain
from math import ceil

from . import test_case
from . import test_data_generation
from .bignum_data import INPUTS_DEFAULT, MODULI_DEFAULT

T = TypeVar('T') #pylint: disable=invalid-name

def invmod(a: int, n: int) -> int:
    """Return inverse of a to modulo n.

    Warning: the output might be negative! See invmod_positive().
    """
    b, c = 1, 0
    while n:
        q, r = divmod(a, n)
        a, b, c, n = n, c, b - q*c, r
    # at this point a is the gcd of the original inputs
    if a == 1:
        return b
    raise ValueError("Not invertible")

def invmod_positive(a: int, n: int) -> int:
    """Return a non-negative inverse of a to modulo n.

    Equivalent to pow(a, -1, n) in Python 3.8+.
    """
    inv = invmod(a, n)
    return inv if inv >= 0 else inv + n

def hex_to_int(val: str) -> int:
    """Implement the syntax accepted by mbedtls_test_read_mpi().

    This is a superset of what is accepted by mbedtls_test_read_mpi_core().
    """
    if val in ['', '-']:
        return 0
    return int(val, 16)

def quote_str(val: str) -> str:
    return "\"{}\"".format(val)

def bound_mpi(val: int, bits_in_limb: int) -> int:
    """First number exceeding number of limbs needed for given input value."""
    return bound_mpi_limbs(limbs_mpi(val, bits_in_limb), bits_in_limb)

def bound_mpi_limbs(limbs: int, bits_in_limb: int) -> int:
    """First number exceeding maximum of given number of limbs."""
    bits = bits_in_limb * limbs
    return 1 << bits

def limbs_mpi(val: int, bits_in_limb: int) -> int:
    """Return the number of limbs required to store value."""
    bit_length = max(val.bit_length(), 1)
    return (bit_length + bits_in_limb - 1) // bits_in_limb

def combination_pairs(values: List[T]) -> List[Tuple[T, T]]:
    """Return all pair combinations from input values."""
    return [(x, y) for x in values for y in values]

def combination_two_lists(first_vals: List[T], second_vals: List[T]) -> List[Tuple[T, T]]:
    """Return all pair combinations from two input lists"""
    return [(x, y) for x in first_vals for y in second_vals]

def expand_list_negative(values: List[str]) -> List[str]:
    """Adds the negative of every element in the list to the list"""
    return values + [f"-{value}" for value in values]

def bits_to_limbs(bits: int, bits_in_limb: int) -> int:
    """ Return the appropriate ammount of limbs needed to store
        a number contained in input bits"""
    return ceil(bits / bits_in_limb)

def hex_digits_for_limb(limbs: int, bits_in_limb: int) -> int:
    """ Return the hex digits need for a number of limbs. """
    return 2 * ((limbs * bits_in_limb) // 8)

def hex_digits_max_int(val: str, bits_in_limb: int) -> int:
    """ Return the first number exceeding maximum  the limb space
    required to store the input hex-string value. This method
    weights on the input str_len rather than numerical value
    and works with zero-padded inputs"""
    n = ((1 << (len(val) * 4)) - 1)
    l = limbs_mpi(n, bits_in_limb)
    return bound_mpi_limbs(l, bits_in_limb)

def zfill_match(reference: str, target: str) -> str:
    """ Zero pad target hex-string to match the limb size of
    the reference input """
    lt = len(target)
    lr = len(reference)
    target_len = lr if lt < lr else lt
    return "{:x}".format(int(target, 16)).zfill(target_len)

class OperationCommon(test_data_generation.BaseTest):
    """Common features for bignum binary operations.

    This adds functionality common in binary operation tests.

    Attributes:
        symbol: Symbol to use for the operation in case description.
        input_values: List of values to use as test case inputs. These are
            combined to produce pairs of values.
        input_cases: List of tuples containing pairs of test case inputs. This
            can be used to implement specific pairs of inputs.
        unique_combinations_only: Boolean to select if test case combinations
            must be unique. If True, only A,B or B,A would be included as a test
            case. If False, both A,B and B,A would be included.
        input_style: Controls the way how test data is passed to the functions
            in the generated test cases. "variable" passes them as they are
            defined in the python source. "arch_split" pads the values with
            zeroes depending on the architecture/limb size. If this is set,
            test cases are generated for all architectures.
        arity: the number of operands for the operation. Currently supported
            values are 1 and 2.
    """
    symbol = ""
    input_values = INPUTS_DEFAULT # type: List[str]
    input_cases = [] # type: List[Any]
    dependencies = [] # type: List[Any]
    unique_combinations_only = False
    input_styles = ["variable", "fixed", "arch_split"] # type: List[str]
    input_style = "variable" # type: str
    limb_sizes = [32, 64] # type: List[int]
    arities = [1, 2]
    arity = 2
    suffix = False   # for arity = 1, symbol can be prefix (default) or suffix

    def __init__(self, val_a: str, val_b: str = "0", bits_in_limb: int = 32) -> None:
        self.val_a = val_a
        self.val_b = val_b
        # Setting the int versions here as opposed to making them @properties
        # provides earlier/more robust input validation.
        self.int_a = hex_to_int(val_a)
        self.int_b = hex_to_int(val_b)
        self.dependencies = deepcopy(self.dependencies)
        if bits_in_limb not in self.limb_sizes:
            raise ValueError("Invalid number of bits in limb!")
        if self.input_style == "arch_split":
            self.dependencies.append("MBEDTLS_HAVE_INT{:d}".format(bits_in_limb))
        self.bits_in_limb = bits_in_limb

    @property
    def boundary(self) -> int:
        if self.arity == 1:
            return self.int_a
        elif self.arity == 2:
            return max(self.int_a, self.int_b)
        raise ValueError("Unsupported number of operands!")

    @property
    def limb_boundary(self) -> int:
        return bound_mpi(self.boundary, self.bits_in_limb)

    @property
    def limbs(self) -> int:
        return limbs_mpi(self.boundary, self.bits_in_limb)

    @property
    def hex_digits(self) -> int:
        return hex_digits_for_limb(self.limbs, self.bits_in_limb)

    def format_arg(self, val: str) -> str:
        if self.input_style not in self.input_styles:
            raise ValueError("Unknown input style!")
        if self.input_style == "variable":
            return val
        else:
            return val.zfill(self.hex_digits)

    def format_result(self, res: int) -> str:
        res_str = '{:x}'.format(res)
        return quote_str(self.format_arg(res_str))

    @property
    def arg_a(self) -> str:
        return self.format_arg(self.val_a)

    @property
    def arg_b(self) -> str:
        if self.arity == 1:
            raise AttributeError("Operation is unary and doesn't have arg_b!")
        return self.format_arg(self.val_b)

    def arguments(self) -> List[str]:
        args = [quote_str(self.arg_a)]
        if self.arity == 2:
            args.append(quote_str(self.arg_b))
        return args + self.result()

    def description(self) -> str:
        """Generate a description for the test case.

        If not set, case_description uses the form A `symbol` B, where symbol
        is used to represent the operation. Descriptions of each value are
        generated to provide some context to the test case.
        """
        if not self.case_description:
            if self.arity == 1:
                format_string = "{1:x} {0}" if self.suffix else "{0} {1:x}"
                self.case_description = format_string.format(
                    self.symbol, self.int_a
                )
            elif self.arity == 2:
                self.case_description = "{:x} {} {:x}".format(
                    self.int_a, self.symbol, self.int_b
                )
        return super().description()

    @property
    def is_valid(self) -> bool:
        return True

    @abstractmethod
    def result(self) -> List[str]:
        """Get the result of the operation.

        This could be calculated during initialization and stored as `_result`
        and then returned, or calculated when the method is called.
        """
        raise NotImplementedError

    @classmethod
    def get_value_pairs(cls) -> Iterator[Tuple[str, str]]:
        """Generator to yield pairs of inputs.

        Combinations are first generated from all input values, and then
        specific cases provided.
        """
        if cls.arity == 1:
            yield from ((a, "0") for a in cls.input_values)
        elif cls.arity == 2:
            if cls.unique_combinations_only:
                yield from combination_pairs(cls.input_values)
            else:
                yield from (
                    (a, b)
                    for a in cls.input_values
                    for b in cls.input_values
                )
        else:
            raise ValueError("Unsupported number of operands!")

    @classmethod
    def generate_function_tests(cls) -> Iterator[test_case.TestCase]:
        if cls.input_style not in cls.input_styles:
            raise ValueError("Unknown input style!")
        if cls.arity not in cls.arities:
            raise ValueError("Unsupported number of operands!")
        if cls.input_style == "arch_split":
            test_objects = (cls(a, b, bits_in_limb=bil)
                            for a, b in cls.get_value_pairs()
                            for bil in cls.limb_sizes)
            special_cases = (cls(*args, bits_in_limb=bil) # type: ignore
                             for args in cls.input_cases
                             for bil in cls.limb_sizes)
        else:
            test_objects = (cls(a, b)
                            for a, b in cls.get_value_pairs())
            special_cases = (cls(*args) for args in cls.input_cases)
        yield from (valid_test_object.create_test_case()
                    for valid_test_object in filter(
                        lambda test_object: test_object.is_valid,
                        chain(test_objects, special_cases)
                        )
                    )


class ModulusRepresentation(enum.Enum):
    """Representation selector of a modulus."""
    # Numerical values aligned with the type mbedtls_mpi_mod_rep_selector
    INVALID = 0
    MONTGOMERY = 2
    OPT_RED = 3

    def symbol(self) -> str:
        """The C symbol for this representation selector."""
        return 'MBEDTLS_MPI_MOD_REP_' + self.name

    @classmethod
    def supported_representations(cls) -> List['ModulusRepresentation']:
        """Return all representations that are supported in positive test cases."""
        return [cls.MONTGOMERY, cls.OPT_RED]


class ModOperationCommon(OperationCommon):
    #pylint: disable=abstract-method
    """Target for bignum mod_raw test case generation."""
    moduli = MODULI_DEFAULT # type: List[str]
    montgomery_form_a = False
    disallow_zero_a = False

    def __init__(self, val_n: str, val_a: str, val_b: str = "0",
                 bits_in_limb: int = 64) -> None:
        super().__init__(val_a=val_a, val_b=val_b, bits_in_limb=bits_in_limb)
        self.val_n = val_n
        # Setting the int versions here as opposed to making them @properties
        # provides earlier/more robust input validation.
        self.int_n = hex_to_int(val_n)

    def to_montgomery(self, val: int) -> int:
        return (val * self.r) % self.int_n

    def from_montgomery(self, val: int) -> int:
        return (val * self.r_inv) % self.int_n

    def convert_from_canonical(self, canonical: int,
                               rep: ModulusRepresentation) -> int:
        """Convert values from canonical representation to the given representation."""
        if rep is ModulusRepresentation.MONTGOMERY:
            return self.to_montgomery(canonical)
        elif rep is ModulusRepresentation.OPT_RED:
            return canonical
        else:
            raise ValueError('Modulus representation not supported: {}'
                             .format(rep.name))

    @property
    def boundary(self) -> int:
        return self.int_n

    @property
    def arg_a(self) -> str:
        if self.montgomery_form_a:
            value_a = self.to_montgomery(self.int_a)
        else:
            value_a = self.int_a
        return self.format_arg('{:x}'.format(value_a))

    @property
    def arg_n(self) -> str:
        return self.format_arg(self.val_n)

    def format_arg(self, val: str) -> str:
        return super().format_arg(val).zfill(self.hex_digits)

    def arguments(self) -> List[str]:
        return [quote_str(self.arg_n)] + super().arguments()

    @property
    def r(self) -> int: # pylint: disable=invalid-name
        l = limbs_mpi(self.int_n, self.bits_in_limb)
        return bound_mpi_limbs(l, self.bits_in_limb)

    @property
    def r_inv(self) -> int:
        return invmod(self.r, self.int_n)

    @property
    def r2(self) -> int: # pylint: disable=invalid-name
        return pow(self.r, 2)

    @property
    def is_valid(self) -> bool:
        if self.int_a >= self.int_n:
            return False
        if self.disallow_zero_a and self.int_a == 0:
            return False
        if self.arity == 2 and self.int_b >= self.int_n:
            return False
        return True

    def description(self) -> str:
        """Generate a description for the test case.

        It uses the form A `symbol` B mod N, where symbol is used to represent
        the operation.
        """

        if not self.case_description:
            return super().description() + " mod {:x}".format(self.int_n)
        return super().description()

    @classmethod
    def input_cases_args(cls) -> Iterator[Tuple[Any, Any, Any]]:
        if cls.arity == 1:
            yield from ((n, a, "0") for a, n in cls.input_cases)
        elif cls.arity == 2:
            yield from ((n, a, b) for a, b, n in cls.input_cases)
        else:
            raise ValueError("Unsupported number of operands!")

    @classmethod
    def generate_function_tests(cls) -> Iterator[test_case.TestCase]:
        if cls.input_style not in cls.input_styles:
            raise ValueError("Unknown input style!")
        if cls.arity not in cls.arities:
            raise ValueError("Unsupported number of operands!")
        if cls.input_style == "arch_split":
            test_objects = (cls(n, a, b, bits_in_limb=bil)
                            for n in cls.moduli
                            for a, b in cls.get_value_pairs()
                            for bil in cls.limb_sizes)
            special_cases = (cls(*args, bits_in_limb=bil)
                             for args in cls.input_cases_args()
                             for bil in cls.limb_sizes)
        else:
            test_objects = (cls(n, a, b)
                            for n in cls.moduli
                            for a, b in cls.get_value_pairs())
            special_cases = (cls(*args) for args in cls.input_cases_args())
        yield from (valid_test_object.create_test_case()
                    for valid_test_object in filter(
                        lambda test_object: test_object.is_valid,
                        chain(test_objects, special_cases)
                        ))
