"""Collect macro definitions from header files.
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#

import itertools
import re
from typing import Dict, IO, Iterable, Iterator, List, Optional, Pattern, Set, Tuple, Union


class ReadFileLineException(Exception):
    def __init__(self, filename: str, line_number: Union[int, str]) -> None:
        message = 'in {} at {}'.format(filename, line_number)
        super(ReadFileLineException, self).__init__(message)
        self.filename = filename
        self.line_number = line_number


class read_file_lines:
    # Dear Pylint, conventionally, a context manager class name is lowercase.
    # pylint: disable=invalid-name,too-few-public-methods
    """Context manager to read a text file line by line.

    ```
    with read_file_lines(filename) as lines:
        for line in lines:
            process(line)
    ```
    is equivalent to
    ```
    with open(filename, 'r') as input_file:
        for line in input_file:
            process(line)
    ```
    except that if process(line) raises an exception, then the read_file_lines
    snippet annotates the exception with the file name and line number.
    """
    def __init__(self, filename: str, binary: bool = False) -> None:
        self.filename = filename
        self.file = None #type: Optional[IO[str]]
        self.line_number = 'entry' #type: Union[int, str]
        self.generator = None #type: Optional[Iterable[Tuple[int, str]]]
        self.binary = binary
    def __enter__(self) -> 'read_file_lines':
        self.file = open(self.filename, 'rb' if self.binary else 'r')
        self.generator = enumerate(self.file)
        return self
    def __iter__(self) -> Iterator[str]:
        assert self.generator is not None
        for line_number, content in self.generator:
            self.line_number = line_number
            yield content
        self.line_number = 'exit'
    def __exit__(self, exc_type, exc_value, exc_traceback) -> None:
        if self.file is not None:
            self.file.close()
        if exc_type is not None:
            raise ReadFileLineException(self.filename, self.line_number) \
                from exc_value


class PSAMacroEnumerator:
    """Information about constructors of various PSA Crypto types.

    This includes macro names as well as information about their arguments
    when applicable.

    This class only provides ways to enumerate expressions that evaluate to
    values of the covered types. Derived classes are expected to populate
    the set of known constructors of each kind, as well as populate
    `self.arguments_for` for arguments that are not of a kind that is
    enumerated here.
    """
    #pylint: disable=too-many-instance-attributes

    def __init__(self) -> None:
        """Set up an empty set of known constructor macros.
        """
        self.statuses = set() #type: Set[str]
        self.lifetimes = set() #type: Set[str]
        self.locations = set() #type: Set[str]
        self.persistence_levels = set() #type: Set[str]
        self.algorithms = set() #type: Set[str]
        self.ecc_curves = set() #type: Set[str]
        self.dh_groups = set() #type: Set[str]
        self.key_types = set() #type: Set[str]
        self.key_usage_flags = set() #type: Set[str]
        self.hash_algorithms = set() #type: Set[str]
        self.mac_algorithms = set() #type: Set[str]
        self.key_agreement_algorithms = set() #type: Set[str]
        self.key_derivation_algorithms = set() #type: Set[str]
        self.pake_algorithms = set() #type: Set[str]
        self.aead_algorithms = set() #type: Set[str]
        self.sign_algorithms = set() #type: Set[str]
        # macro name -> list of argument names
        self.argspecs = {} #type: Dict[str, List[str]]
        # argument name -> list of values
        self.arguments_for = {
            'mac_length': [],
            'min_mac_length': [],
            'tag_length': [],
            'min_tag_length': [],
        } #type: Dict[str, List[str]]
        # Whether to include intermediate macros in enumerations. Intermediate
        # macros serve as category headers and are not valid values of their
        # type. See `is_internal_name`.
        # Always false in this class, may be set to true in derived classes.
        self.include_intermediate = False
        # Deprecated backward compatibility alias for generate_psa_constants.py.
        self.ka_algorithms = self.key_agreement_algorithms

    def is_internal_name(self, name: str) -> bool:
        """Whether this is an internal macro. Internal macros will be skipped."""
        if not self.include_intermediate:
            if name.endswith('_BASE') or name.endswith('_NONE'):
                return True
            if '_CATEGORY_' in name:
                return True
        return name.endswith('_FLAG') or name.endswith('_MASK')

    def gather_arguments(self) -> None:
        """Populate the list of values for macro arguments.

        Call this after parsing all the inputs.
        """
        self.arguments_for['hash_alg'] = sorted(self.hash_algorithms)
        self.arguments_for['mac_alg'] = sorted(self.mac_algorithms)
        self.arguments_for['ka_alg'] = sorted(self.key_agreement_algorithms)
        self.arguments_for['kdf_alg'] = sorted(self.key_derivation_algorithms)
        self.arguments_for['aead_alg'] = sorted(self.aead_algorithms)
        self.arguments_for['sign_alg'] = sorted(self.sign_algorithms)
        self.arguments_for['curve'] = sorted(self.ecc_curves)
        self.arguments_for['group'] = sorted(self.dh_groups)
        self.arguments_for['persistence'] = sorted(self.persistence_levels)
        self.arguments_for['location'] = sorted(self.locations)
        self.arguments_for['lifetime'] = sorted(self.lifetimes)

    @staticmethod
    def _format_arguments(name: str, arguments: Iterable[str]) -> str:
        """Format a macro call with arguments.

        The resulting format is consistent with
        `InputsForTest.normalize_argument`.
        """
        return name + '(' + ', '.join(arguments) + ')'

    _argument_split_re = re.compile(r' *, *')
    @classmethod
    def _argument_split(cls, arguments: str) -> List[str]:
        return re.split(cls._argument_split_re, arguments)

    def distribute_arguments(self, name: str) -> Iterator[str]:
        """Generate macro calls with each tested argument set.

        If name is a macro without arguments, just yield "name".
        If name is a macro with arguments, yield a series of
        "name(arg1,...,argN)" where each argument takes each possible
        value at least once.
        """
        try:
            if name not in self.argspecs:
                yield name
                return
            argspec = self.argspecs[name]
            if argspec == []:
                yield name + '()'
                return
            argument_lists = [self.arguments_for[arg] for arg in argspec]
            arguments = [values[0] for values in argument_lists]
            yield self._format_arguments(name, arguments)
            # Dear Pylint, enumerate won't work here since we're modifying
            # the array.
            # pylint: disable=consider-using-enumerate
            for i in range(len(arguments)):
                for value in argument_lists[i][1:]:
                    arguments[i] = value
                    yield self._format_arguments(name, arguments)
                arguments[i] = argument_lists[i][0]
        except BaseException as e:
            raise Exception('distribute_arguments({})'.format(name)) from e

    def distribute_arguments_without_duplicates(
            self, seen: Set[str], name: str
    ) -> Iterator[str]:
        """Same as `distribute_arguments`, but don't repeat seen results."""
        for result in self.distribute_arguments(name):
            if result not in seen:
                seen.add(result)
                yield result

    def generate_expressions(self, names: Iterable[str]) -> Iterator[str]:
        """Generate expressions covering values constructed from the given names.

        `names` can be any iterable collection of macro names.

        For example:
        * ``generate_expressions(['PSA_ALG_CMAC', 'PSA_ALG_HMAC'])``
          generates ``'PSA_ALG_CMAC'`` as well as ``'PSA_ALG_HMAC(h)'`` for
          every known hash algorithm ``h``.
        * ``macros.generate_expressions(macros.key_types)`` generates all
          key types.
        """
        seen = set() #type: Set[str]
        return itertools.chain(*(
            self.distribute_arguments_without_duplicates(seen, name)
            for name in names
        ))


class PSAMacroCollector(PSAMacroEnumerator):
    """Collect PSA crypto macro definitions from C header files.
    """

    def __init__(self, include_intermediate: bool = False) -> None:
        """Set up an object to collect PSA macro definitions.

        Call the read_file method of the constructed object on each header file.

        * include_intermediate: if true, include intermediate macros such as
          PSA_XXX_BASE that do not designate semantic values.
        """
        super().__init__()
        self.include_intermediate = include_intermediate
        self.key_types_from_curve = {} #type: Dict[str, str]
        self.key_types_from_group = {} #type: Dict[str, str]
        self.algorithms_from_hash = {} #type: Dict[str, str]

    @staticmethod
    def algorithm_tester(name: str) -> str:
        """The predicate for whether an algorithm is built from the given constructor.

        The given name must be the name of an algorithm constructor of the
        form ``PSA_ALG_xxx`` which is used as ``PSA_ALG_xxx(yyy)`` to build
        an algorithm value. Return the corresponding predicate macro which
        is used as ``predicate(alg)`` to test whether ``alg`` can be built
        as ``PSA_ALG_xxx(yyy)``. The predicate is usually called
        ``PSA_ALG_IS_xxx``.
        """
        prefix = 'PSA_ALG_'
        assert name.startswith(prefix)
        midfix = 'IS_'
        suffix = name[len(prefix):]
        if suffix in ['DSA', 'ECDSA']:
            midfix += 'RANDOMIZED_'
        elif suffix == 'RSA_PSS':
            suffix += '_STANDARD_SALT'
        return prefix + midfix + suffix

    def record_algorithm_subtype(self, name: str, expansion: str) -> None:
        """Record the subtype of an algorithm constructor.

        Given a ``PSA_ALG_xxx`` macro name and its expansion, if the algorithm
        is of a subtype that is tracked in its own set, add it to the relevant
        set.
        """
        # This code is very ad hoc and fragile. It should be replaced by
        # something more robust.
        if re.match(r'MAC(?:_|\Z)', name):
            self.mac_algorithms.add(name)
        elif re.match(r'KDF(?:_|\Z)', name):
            self.key_derivation_algorithms.add(name)
        elif re.search(r'0x020000[0-9A-Fa-f]{2}', expansion):
            self.hash_algorithms.add(name)
        elif re.search(r'0x03[0-9A-Fa-f]{6}', expansion):
            self.mac_algorithms.add(name)
        elif re.search(r'0x05[0-9A-Fa-f]{6}', expansion):
            self.aead_algorithms.add(name)
        elif re.search(r'0x09[0-9A-Fa-f]{2}0000', expansion):
            self.key_agreement_algorithms.add(name)
        elif re.search(r'0x08[0-9A-Fa-f]{6}', expansion):
            self.key_derivation_algorithms.add(name)

    # "#define" followed by a macro name with either no parameters
    # or a single parameter and a non-empty expansion.
    # Grab the macro name in group 1, the parameter name if any in group 2
    # and the expansion in group 3.
    _define_directive_re = re.compile(r'\s*#\s*define\s+(\w+)' +
                                      r'(?:\s+|\((\w+)\)\s*)' +
                                      r'(.+)')
    _deprecated_definition_re = re.compile(r'\s*MBEDTLS_DEPRECATED')

    # Macro that is a destructor, not a constructor (i.e. takes a thing as
    # an argument and analyzes it, rather than constructing a thing).
    _destructor_name_re = re.compile('|'.join([
        r'.*(?:_GET_|_HAS_|_IS_)',
        r'.*_LENGTH\Z',
        r'PSA_ALG_SIGN_SUPPORTS_CONTEXT\Z',
    ]))

    # Macro that converts between things, rather than building a thing from
    # scratch.
    _conversion_macro_names = frozenset([
        'PSA_KEY_TYPE_KEY_PAIR_OF_PUBLIC_KEY',
        'PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR',
        'PSA_ALG_FULL_LENGTH_MAC',
        'PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG',
        'PSA_JPAKE_EXPECTED_INPUTS',
        'PSA_JPAKE_EXPECTED_OUTPUTS',
    ])

    def read_line(self, line):
        """Parse a C header line and record the PSA identifier it defines if any.
        This function analyzes lines that start with "#define PSA_"
        (up to non-significant whitespace) and skips all non-matching lines.
        """
        # pylint: disable=too-many-branches,too-many-return-statements
        m = re.match(self._define_directive_re, line)
        if not m:
            return
        name, parameter, expansion = m.groups()
        expansion = re.sub(r'/\*.*?\*/|//.*', r' ', expansion)
        if parameter:
            self.argspecs[name] = [parameter]
        if re.match(self._deprecated_definition_re, expansion):
            # Skip deprecated values, which are assumed to be
            # backward compatibility aliases that share
            # numerical values with non-deprecated values.
            return
        if re.match(self._destructor_name_re, name):
            # Not a constructor
            return
        if name in self._conversion_macro_names:
            # Not a constructor
            return
        if self.is_internal_name(name):
            # Macro only to build actual values
            return
        elif (name.startswith('PSA_ERROR_') or name == 'PSA_SUCCESS') \
           and not parameter:
            self.statuses.add(name)
        elif name.startswith('PSA_KEY_TYPE_') and not parameter:
            self.key_types.add(name)
        elif name.startswith('PSA_KEY_TYPE_') and parameter == 'curve':
            self.key_types_from_curve[name] = name[:13] + 'IS_' + name[13:]
        elif name.startswith('PSA_KEY_TYPE_') and parameter == 'group':
            self.key_types_from_group[name] = name[:13] + 'IS_' + name[13:]
        elif name.startswith('PSA_ECC_FAMILY_') and not parameter:
            self.ecc_curves.add(name)
        elif name.startswith('PSA_DH_FAMILY_') and not parameter:
            self.dh_groups.add(name)
        elif name.startswith('PSA_ALG_') and not parameter:
            if name in ['PSA_ALG_ECDSA_BASE',
                        'PSA_ALG_RSA_PKCS1V15_SIGN_BASE']:
                # Ad hoc skipping of duplicate names for some numerical values
                return
            self.algorithms.add(name)
            self.record_algorithm_subtype(name, expansion)
        elif name.startswith('PSA_ALG_') and parameter == 'hash_alg':
            self.algorithms_from_hash[name] = self.algorithm_tester(name)
        elif name.startswith('PSA_KEY_USAGE_') and not parameter:
            self.key_usage_flags.add(name)
        elif parameter is None:
            # Macro with no parameter, whose name does not start with one
            # of the prefixes we look for. Just ignore it.
            return
        else:
            raise Exception("Unsupported macro and parameter name: {}({})"
                            .format(name, parameter))

    _nonascii_re = re.compile(rb'[^\x00-\x7f]+')
    _continued_line_re = re.compile(rb'\\\r?\n\Z')
    def read_file(self, header_file):
        for line in header_file:
            m = re.search(self._continued_line_re, line)
            while m:
                cont = next(header_file)
                line = line[:m.start(0)] + cont
                m = re.search(self._continued_line_re, line)
            line = re.sub(self._nonascii_re, rb'', line).decode('ascii')
            self.read_line(line)


class InputsForTest(PSAMacroEnumerator):
    # pylint: disable=too-many-instance-attributes
    """Accumulate information about macros to test.
enumerate
    This includes macro names as well as information about their arguments
    when applicable.
    """

    def __init__(self) -> None:
        super().__init__()
        self.all_declared = set() #type: Set[str]
        # Identifier prefixes
        self.table_by_prefix = {
            'ERROR': self.statuses,
            'ALG': self.algorithms,
            'ECC_CURVE': self.ecc_curves,
            'DH_GROUP': self.dh_groups,
            'KEY_LIFETIME': self.lifetimes,
            'KEY_LOCATION': self.locations,
            'KEY_PERSISTENCE': self.persistence_levels,
            'KEY_TYPE': self.key_types,
            'KEY_USAGE': self.key_usage_flags,
        } #type: Dict[str, Set[str]]
        # Test functions
        self.table_by_test_function = {
            # Any function ending in _algorithm also gets added to
            # self.algorithms.
            'key_type': [self.key_types],
            'block_cipher_key_type': [self.key_types],
            'stream_cipher_key_type': [self.key_types],
            'ecc_key_family': [self.ecc_curves],
            'ecc_key_types': [self.ecc_curves],
            'dh_key_family': [self.dh_groups],
            'dh_key_types': [self.dh_groups],
            'hash_algorithm': [self.hash_algorithms],
            'mac_algorithm': [self.mac_algorithms],
            'cipher_algorithm': [],
            'hmac_algorithm': [self.mac_algorithms, self.sign_algorithms],
            'aead_algorithm': [self.aead_algorithms],
            'key_derivation_algorithm': [self.key_derivation_algorithms],
            'key_agreement_algorithm': [self.key_agreement_algorithms],
            'asymmetric_signature_algorithm': [self.sign_algorithms],
            'asymmetric_signature_wildcard': [self.algorithms],
            'asymmetric_encryption_algorithm': [],
            'pake_algorithm': [self.pake_algorithms],
            'key_wrap_algorithm': [],
            'key_encapsulation_algorithm': [],
            'xof_algorithm': [],
            'other_algorithm': [],
            'lifetime': [self.lifetimes],
        } #type: Dict[str, List[Set[str]]]
        mac_lengths = [str(n) for n in [
            1,  # minimum expressible
            4,  # minimum allowed by policy
            13, # an odd size in a plausible range
            14, # an even non-power-of-two size in a plausible range
            16, # same as full size for at least one algorithm
            63, # maximum expressible
        ]]
        self.arguments_for['mac_length'] += mac_lengths
        self.arguments_for['min_mac_length'] += mac_lengths
        aead_lengths = [str(n) for n in [
            1,  # minimum expressible
            4,  # minimum allowed by policy
            13, # an odd size in a plausible range
            14, # an even non-power-of-two size in a plausible range
            16, # same as full size for at least one algorithm
            63, # maximum expressible
        ]]
        self.arguments_for['tag_length'] += aead_lengths
        self.arguments_for['min_tag_length'] += aead_lengths

    def add_numerical_values(self) -> None:
        """Add numerical values that are not supported to the known identifiers."""
        # Sets of names per type
        self.algorithms.add('0xffffffff')
        self.ecc_curves.add('0xff')
        self.dh_groups.add('0xff')
        self.key_types.add('0xffff')
        self.key_usage_flags.add('0x80000000')

        # Hard-coded values for unknown algorithms
        #
        # These have to have values that are correct for their respective
        # PSA_ALG_IS_xxx macros, but are also not currently assigned and are
        # not likely to be assigned in the near future.
        self.hash_algorithms.add('0x020000fe') # 0x020000ff is PSA_ALG_ANY_HASH
        self.mac_algorithms.add('0x03007fff')
        self.key_agreement_algorithms.add('0x09fc0000')
        self.key_derivation_algorithms.add('0x080000ff')
        self.pake_algorithms.add('0x0a0000ff')
        # For AEAD algorithms, the only variability is over the tag length,
        # and this only applies to known algorithms, so don't test an
        # unknown algorithm.

    def get_names(self, type_word: str) -> Set[str]:
        """Return the set of known names of values of the given type."""
        return {
            'status': self.statuses,
            'algorithm': self.algorithms,
            'ecc_curve': self.ecc_curves,
            'dh_group': self.dh_groups,
            'key_type': self.key_types,
            'key_usage': self.key_usage_flags,
        }[type_word]

    # Regex for interesting header lines.
    # Groups: 1=macro name, 2=type, 3=argument list (optional).
    _header_line_re = \
        re.compile(r'#define +' +
                   r'(PSA_((?:(?:DH|ECC|KEY)_)?[A-Z]+)_\w+)' +
                   r'(?:\(([^\n()]*)\))?')
    # Regex of macro names to exclude.
    _excluded_name_re = re.compile(r'_(?:GET|HAS|IS|OF)_|_(?:BASE|FLAG|MASK)\Z')
    # Additional excluded macros.
    _excluded_names = set([
        # Macros that provide an alternative way to build the same
        # algorithm as another macro.
        'PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG',
        'PSA_ALG_FULL_LENGTH_MAC',
        # Auxiliary macro whose name doesn't fit the usual patterns for
        # auxiliary macros.
        'PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG_CASE',
    ])
    def parse_header_line(self, line: str) -> None:
        """Parse a C header line, looking for "#define PSA_xxx"."""
        m = re.match(self._header_line_re, line)
        if not m:
            return
        name = m.group(1)
        self.all_declared.add(name)
        if re.search(self._excluded_name_re, name) or \
           name in self._excluded_names or \
           self.is_internal_name(name):
            return
        dest = self.table_by_prefix.get(m.group(2))
        if dest is None:
            return
        dest.add(name)
        if m.group(3):
            self.argspecs[name] = self._argument_split(m.group(3))

    _nonascii_re = re.compile(rb'[^\x00-\x7f]+') #type: Pattern
    def parse_header(self, filename: str) -> None:
        """Parse a C header file, looking for "#define PSA_xxx"."""
        with read_file_lines(filename, binary=True) as lines:
            for line in lines:
                line = re.sub(self._nonascii_re, rb'', line).decode('ascii')
                self.parse_header_line(line)

    _macro_identifier_re = re.compile(r'[A-Z]\w+')
    def generate_undeclared_names(self, expr: str) -> Iterable[str]:
        for name in re.findall(self._macro_identifier_re, expr):
            if name not in self.all_declared:
                yield name

    def accept_test_case_line(self, function: str, argument: str) -> bool:
        #pylint: disable=unused-argument
        undeclared = list(self.generate_undeclared_names(argument))
        if undeclared:
            raise Exception('Undeclared names in test case', undeclared)
        return True

    @staticmethod
    def normalize_argument(argument: str) -> str:
        """Normalize whitespace in the given C expression.

        The result uses the same whitespace as
        ` PSAMacroEnumerator.distribute_arguments`.
        """
        return re.sub(r',', r', ', re.sub(r' +', r'', argument))

    def add_test_case_line(self, function: str, argument: str) -> None:
        """Parse a test case data line, looking for algorithm metadata tests."""
        sets = []
        if function.endswith('_algorithm'):
            sets.append(self.algorithms)
            if function == 'key_agreement_algorithm' and \
               argument.startswith('PSA_ALG_KEY_AGREEMENT('):
                # We only want *raw* key agreement algorithms as such, so
                # exclude ones that are already chained with a KDF.
                # Keep the expression as one to test as an algorithm.
                function = 'other_algorithm'
        sets += self.table_by_test_function[function]
        if self.accept_test_case_line(function, argument):
            for s in sets:
                s.add(self.normalize_argument(argument))

    # Regex matching a *.data line containing a test function call and
    # its arguments. The actual definition is partly positional, but this
    # regex is good enough in practice.
    _test_case_line_re = re.compile(r'(?!depends_on:)(\w+):([^\n :][^:\n]*)')
    def parse_test_cases(self, filename: str) -> None:
        """Parse a test case file (*.data), looking for algorithm metadata tests."""
        with read_file_lines(filename) as lines:
            for line in lines:
                m = re.match(self._test_case_line_re, line)
                if m:
                    self.add_test_case_line(m.group(1), m.group(2))
