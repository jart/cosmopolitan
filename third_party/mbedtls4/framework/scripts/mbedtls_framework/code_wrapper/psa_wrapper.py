"""Generate wrapper functions for PSA function calls.
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

import itertools
import os
from typing import Iterable, Iterator, List, Optional, Tuple

from .. import build_tree
from .. import c_parsing_helper
from .. import c_wrapper_generator
from .. import typing_util

from .psa_buffer import BufferParameter

class PSAWrapperConfiguration:
    """Configuration data class for PSA Wrapper."""
    #pylint: disable=too-few-public-methods

    def __init__(self) -> None:
        self.cpp_guards = [
            "MBEDTLS_PSA_CRYPTO_C",
            "MBEDTLS_TEST_HOOKS",
            "!RECORD_PSA_STATUS_COVERAGE_LOG",
        ]

        self.skipped_functions = frozenset([
            'mbedtls_psa_external_get_random', # not a library function
            'psa_get_key_domain_parameters', # client-side function
            'psa_get_key_slot_number', # client-side function
            'psa_key_derivation_verify_bytes', # not implemented yet
            'psa_key_derivation_verify_key', # not implemented yet
            'psa_set_key_domain_parameters', # client-side function
        ])

        self.skipped_argument_types = frozenset([
            # PAKE stuff: not implemented yet
            'psa_crypto_driver_pake_inputs_t *',
            'psa_pake_cipher_suite_t *',
        ])

        self.function_guards = {
            'mbedtls_psa_register_se_key': 'defined(MBEDTLS_PSA_CRYPTO_SE_C)',
            'mbedtls_psa_inject_entropy': 'defined(MBEDTLS_PSA_INJECT_ENTROPY)',
            'mbedtls_psa_external_get_random': 'defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG)',
            'mbedtls_psa_platform_get_builtin_key': 'defined(MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS)',
            'psa_crypto_driver_pake_get_cipher_suite' : 'defined(PSA_WANT_ALG_SOME_PAKE)',
            'psa_crypto_driver_pake_get_password' : 'defined(PSA_WANT_ALG_SOME_PAKE)',
            'psa_crypto_driver_pake_get_password_len' : 'defined(PSA_WANT_ALG_SOME_PAKE)',
            'psa_crypto_driver_pake_get_peer' : 'defined(PSA_WANT_ALG_SOME_PAKE)',
            'psa_crypto_driver_pake_get_peer_len' : 'defined(PSA_WANT_ALG_SOME_PAKE)',
            'psa_crypto_driver_pake_get_user' : 'defined(PSA_WANT_ALG_SOME_PAKE)',
            'psa_crypto_driver_pake_get_user_len' : 'defined(PSA_WANT_ALG_SOME_PAKE)',
            'psa_pake_abort' : 'defined(PSA_WANT_ALG_SOME_PAKE)',
            'psa_pake_get_implicit_key' : 'defined(PSA_WANT_ALG_SOME_PAKE)',
            'psa_pake_input' : 'defined(PSA_WANT_ALG_SOME_PAKE)',
            'psa_pake_output' : 'defined(PSA_WANT_ALG_SOME_PAKE)',
            'psa_pake_set_password_key' : 'defined(PSA_WANT_ALG_SOME_PAKE)',
            'psa_pake_set_peer' : 'defined(PSA_WANT_ALG_SOME_PAKE)',
            'psa_pake_set_role' : 'defined(PSA_WANT_ALG_SOME_PAKE)',
            'psa_pake_set_user' : 'defined(PSA_WANT_ALG_SOME_PAKE)',
            'psa_pake_setup' : 'defined(PSA_WANT_ALG_SOME_PAKE)',
        }

class PSAWrapper(c_wrapper_generator.Base):
    """Generate a C source file containing wrapper functions for PSA Crypto API calls."""

    _WRAPPER_NAME_PREFIX = 'mbedtls_test_wrap_'
    _WRAPPER_NAME_SUFFIX = ''

    _PSA_WRAPPER_INCLUDES = ['<psa/crypto.h>']
    _DEFAULT_IN_HEADERS = ['crypto.h', 'crypto_extra.h']

    def __init__(self,
                 out_h_f: str,
                 out_c_f: str,
                 in_headers: Optional[List[str]] = None,
                 config: PSAWrapperConfiguration = PSAWrapperConfiguration()) -> None:

        super().__init__()
        self.out_c_f = out_c_f
        self.out_h_f = out_h_f

        self.project_root = build_tree.guess_project_root()
        self.read_config(config)
        self.read_headers(in_headers)

    def read_config(self, cfg: PSAWrapperConfiguration)-> None:
        """Configure instance's parameters from a user provided config."""

        self._cpp_guards = PSAWrapper.parse_def_guards(cfg.cpp_guards)
        self._skip_functions = cfg.skipped_functions
        self._function_guards.update(cfg.function_guards)
        self._not_implemented = cfg.skipped_argument_types

    def read_headers(self, headers: Optional[List[str]]) -> None:
        """Reads functions to be wrapped from source header files into self.functions."""
        self.in_headers = self._DEFAULT_IN_HEADERS if headers is None else headers
        for header_name in self.in_headers:
            header_path = self.rel_path(header_name)
            c_parsing_helper.read_function_declarations(self.functions, header_path)

    def rel_path(self, filename: str, path_list: Optional[List[str]] = None) -> str:
        """Return the estimated path in relationship to the project_root.

           The method allows overriding the targetted sub-directory.
           Currently the default is set to project_root/include/psa."""
        if path_list is None:
            path_list = ['include', 'psa']
        return os.path.join(self.project_root, *path_list, filename)

    # Utility Methods
    @staticmethod
    def parse_def_guards(def_list: Iterable[str])-> str:
        """ Create define guards.

            Convert an input list of into a C preprocessor
            expression of defined() && !defined() syntax string."""

        output = ""
        dl = [("defined({})".format(n) if n[0] != "!" else
               "!defined({})".format(n[1:]))
              for n in def_list]

        # Split the list in chunks of 2 and add new lines
        for i in range(0, len(dl), 2):
            output += "{} && {} && \\".format(dl[i], dl[i+1]) + "\n    "\
                if i+2 <= len(dl) else dl[i]
        return output

    @staticmethod
    def _detect_buffer_parameters(arguments: List[c_parsing_helper.ArgumentInfo],
                                  argument_names: List[str]) -> Iterator[BufferParameter]:
        """Detect function arguments that are buffers (pointer, size [,length])."""
        types = ['' if arg.suffix else arg.type for arg in arguments]
        # pairs = list of (type_of_arg_N, type_of_arg_N+1)
        # where each type_of_arg_X is the empty string if the type is an array
        # or there is no argument X.
        pairs = enumerate(itertools.zip_longest(types, types[1:], fillvalue=''))
        for i, t01 in pairs:
            if (t01[0] == 'const uint8_t *' or t01[0] == 'uint8_t *') and \
               t01[1] == 'size_t':
                yield BufferParameter(i, not t01[0].startswith('const '),
                                      argument_names[i], argument_names[i+1])

    @staticmethod
    def _parameter_should_be_copied(function_name: str,
                                    _buffer_name: Optional[str]) -> bool:
        """Whether the specified buffer argument to a PSA function should be copied.
        """
        # False-positives that do not need buffer copying
        if function_name in ('mbedtls_psa_inject_entropy',
                             'psa_crypto_driver_pake_get_password',
                             'psa_crypto_driver_pake_get_user',
                             'psa_crypto_driver_pake_get_peer'):
            return False

        return True

    @staticmethod
    def _poison_wrap(param: BufferParameter, poison: bool,
                     ident_lv: int = 1) -> str:
        """Returns a call to MBEDTLS_TEST_MEMORY_[UN]POISON.

           The output is prefixed with MBEDTLS_TEST_MEMORY_ followed by POISON/UNPOISON
           and the input parameter arguments (name, length)
        """
        return "{}MBEDTLS_TEST_MEMORY_{}({}, {});\n".format(
            (ident_lv * 4) * ' ',
            'POISON' if poison else 'UNPOISON',
            param.buffer_name, param.size_name)

    def _poison_multi_write(self,
                            out: typing_util.Writable,
                            buffer_parameters: List['BufferParameter'],
                            poison: bool) -> None:
        """Write poisoning or unpoisoning code for the buffer parameters.

           Write poisoning code if poison is true, unpoisoning code otherwise.
        """

        if not buffer_parameters:
            return
        out.write('#if !defined(MBEDTLS_PSA_ASSUME_EXCLUSIVE_BUFFERS)\n')
        for param in buffer_parameters:
            out.write(self._poison_wrap(param, poison))
        out.write('#endif /* !defined(MBEDTLS_PSA_ASSUME_EXCLUSIVE_BUFFERS) */\n')

    # Override parent's methods
    def _write_function_call(self, out: typing_util.Writable,
                             function: c_wrapper_generator.FunctionInfo,
                             argument_names: List[str]) -> None:
        buffer_parameters = list(
            param
            for param in self._detect_buffer_parameters(function.arguments,
                                                        argument_names)
            if self._parameter_should_be_copied(function.name,
                                                function.arguments[param.index].name))

        self._poison_multi_write(out, buffer_parameters, True)
        super()._write_function_call(out, function, argument_names)
        self._poison_multi_write(out, buffer_parameters, False)

    def _skip_function(self, function: c_wrapper_generator.FunctionInfo) -> bool:
        if function.return_type != 'psa_status_t':
            return True
        if function.name in self._skip_functions:
            return True
        return False

    def _return_variable_name(self,
                              function: c_wrapper_generator.FunctionInfo) -> str:
        """The name of the variable that will contain the return value."""

        if function.return_type == 'psa_status_t':
            return 'status'
        return super()._return_variable_name(function)

    def _write_prologue(self, out: typing_util.Writable, header: bool) -> None:
        super()._write_prologue(out, header)

        prologue = []
        if self._cpp_guards:
            prologue.append("#if {}".format(self._cpp_guards))
            prologue.append('')

        for include in self._PSA_WRAPPER_INCLUDES:
            prologue.append("#include {}".format(include))

        # Make certain there is an empty line at the end of this section.
        for i in [-1, -2]:
            if prologue[i] != '':
                prologue.append('')

        out.write("\n".join(prologue))

    def _write_epilogue(self, out: typing_util.Writable, header: bool) -> None:
        if self._cpp_guards:
            out.write("#endif /* {} */\n\n".format(self._cpp_guards))
        super()._write_epilogue(out, header)

class PSALoggingWrapper(PSAWrapper, c_wrapper_generator.Logging):
    """Generate a C source file containing wrapper functions that log PSA Crypto API calls."""

    def __init__(self, #pylint: disable=too-many-arguments
                 stream: str,
                 out_h_f: str,
                 out_c_f: str,
                 in_headers: Optional[List[str]] = None,
                 config: PSAWrapperConfiguration = PSAWrapperConfiguration()) -> None:

        super().__init__(out_h_f, out_c_f, in_headers, config)
        self.set_stream(stream)

    _PRINTF_TYPE_CAST = c_wrapper_generator.Logging._PRINTF_TYPE_CAST.copy()
    _PRINTF_TYPE_CAST.update({
        'mbedtls_svc_key_id_t': 'unsigned',
        'psa_algorithm_t': 'unsigned',
        'psa_drv_slot_number_t': 'unsigned long long',
        'psa_key_derivation_step_t': 'int',
        'psa_key_id_t': 'unsigned',
        'psa_key_slot_number_t': 'unsigned long long',
        'psa_key_lifetime_t': 'unsigned',
        'psa_key_type_t': 'unsigned',
        'psa_key_usage_flags_t': 'unsigned',
        'psa_pake_role_t': 'int',
        'psa_pake_step_t': 'int',
        'psa_status_t': 'int',
    })

    def _printf_parameters(self, typ: str, var: str) -> Tuple[str, List[str]]:
        if typ.startswith('const '):
            typ = typ[6:]
        if typ == 'uint8_t *':
            # Skip buffers
            return '', []
        if typ.endswith('operation_t *'):
            return '', []
        if typ in self._not_implemented:
            return '', []
        if typ == 'psa_key_attributes_t *':
            return (var + '={id=%u, lifetime=0x%08x, type=0x%08x, bits=%u, alg=%08x, usage=%08x}',
                    ['(unsigned) psa_get_key_{}({})'.format(field, var)
                     for field in ['id', 'lifetime', 'type', 'bits', 'algorithm', 'usage_flags']])
        return super()._printf_parameters(typ, var)
