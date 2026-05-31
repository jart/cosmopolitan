"""Generate C preprocessor code to check for bad configurations.

The headers are meant to be included in a specific way in PROJECT_config.c.
See framework/docs/architecture/config-check-framework.md for information.
"""

## Copyright The Mbed TLS Contributors
## SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

import argparse
import enum
import os
import posixpath
import re
import sys
import textwrap
import typing
from typing import Iterator, List, Optional

from . import build_tree
from . import typing_util


class Position(enum.Enum):
    BEFORE = 0 # Before build_info.h
    USER = 1 # Just after reading PROJECT_CONFIG_FILE (*config.h) and PROJECT_USER_CONFIG_FILE
    FINAL = 2 # After *adjust*.h and the rest of build_info.h


class Checker:
    """Description of checks for one option.

    By default, this class triggers an error if the option is set after
    reading the user configuration. To change the behavior, override
    the methods `before`, `user` and `final` as needed.
    """

    def __init__(self, name: str, suggestion: str = '') -> None:
        """Construct a checker for the given preprocessor macro name.

        If suggestion is given, it is appended to the error message.
        It should be a short sentence intended for human readers.
        This sentence follows a sentence like "<macro_name> is not
        a valid configuration option".
        """
        self.name = name
        self.suggestion = suggestion

    def _basic_message(self) -> str:
        """The first sentence of the message to display on error.

        It should end with a full stop or other sentence-ending punctuation.
        """
        return f'{self.name} is not a valid configuration option.'

    def message(self) -> str:
        """The message to display on error."""
        message = self._basic_message()
        if self.suggestion:
            message += ' Suggestion: ' + self.suggestion
        return message

    def _quoted_message(self) -> str:
        """Quote message() in double quotes. Useful for #error directives."""
        return ('"' +
                str.replace(str.replace(self.message(),
                                        '\\', '\\\\'),
                            '"', '\\"') +
                '"')

    def before(self, _prefix: str) -> str:
        """C code to inject before including the config."""
        #pylint: disable=no-self-use
        # Derived classes will add content where needed.
        return ''

    def user(self, _prefix: str) -> str:
        """C code to inject immediately after including the user config."""
        return f'''
        #if defined({self.name})
        #  error {self._quoted_message()}
        #endif
        '''

    def final(self, _prefix: str) -> str:
        """C code to inject after finalizing the config."""
        #pylint: disable=no-self-use
        # Derived classes will add content where needed.
        return ''

    def code(self, position: Position, prefix: str) -> str:
        """C code to inject at the given position.

        Use the given prefix for auxiliary macro names.
        """
        methods = {
            Position.BEFORE: self.before,
            Position.USER: self.user,
            Position.FINAL: self.final,
        }
        method = methods[position]
        snippet = method(prefix)
        return textwrap.dedent(snippet)


class Internal(Checker):
    """Checker for an internal-only option."""


class SubprojectInternal(Checker):
    """Checker for an internal macro of a subproject."""

    # must be overridden in child classes
    SUBPROJECT = None #type: Optional[str]

    def __init__(self, name: str, suggestion: str = '') -> None:
        super().__init__(name, suggestion)
        if self.SUBPROJECT is None:
            raise ValueError('No subproject specified for ' + name)
        self.subproject = self.SUBPROJECT #type: str

    def _basic_message(self) -> str:
        return f'{self.name} is an internal macro of {self.subproject} and may not be configured.'

    def before(self, prefix: str) -> str:
        return f'''
        #if defined({self.name})
        #  define {prefix}_WASSET_{self.name} 1
        #else
        #  define {prefix}_WASSET_{self.name} 0
        #endif
        #undef {self.name}
        '''

    def user(self, prefix: str) -> str:
        return f'''
        #if defined({self.name})
        #  error {self._quoted_message()}
        #endif
        #if {prefix}_WASSET_{self.name}
        #  define {self.name}
        #endif
        #undef {prefix}_WASSET_{self.name}
        '''


class SubprojectOption(SubprojectInternal):
    """Checker for a configuration option of a subproject."""

    def _basic_message(self) -> str:
        return f'{self.name} must be configured in {self.subproject}.'

    def user(self, prefix: str) -> str:
        return f'''
        #if defined({self.name}) && !{prefix}_WASSET_{self.name}
        #  error {self._quoted_message()}
        #endif
        #if {prefix}_WASSET_{self.name}
        #  define {self.name}
        #endif
        #undef {prefix}_WASSET_{self.name}
        '''


class Removed(Checker):
    """Checker for an option that has been removed."""

    def __init__(self, name: str, version: str, suggestion: str = '') -> None:
        super().__init__(name, suggestion)
        self.version = version

    def _basic_message(self) -> str:
        """The first sentence of the message to display on error.

        It should end with a full stop or other sentence-ending punctuation.
        """
        return f'{self.name} was removed in {self.version}.'

    def user(self, prefix: str) -> str:
        """C code to inject immediately after including the user config."""
        # A removed option is forbidden, just like an internal option.
        # But since we're checking a macro that is not defined anywhere,
        # we need to tell check_names.py that this is a false positive.
        code = super().user(prefix)
        return re.sub(rf'^ *# *\w+.*\b{self.name}\b.*$',
                      lambda m: m.group(0) + ' //no-check-names',
                      code, flags=re.M)


class BranchData(typing.NamedTuple):
    """The relevant aspects of the configuration on a branch."""

    # Subdirectory where the generated headers will be located.
    header_directory: str

    # Prefix used for the generated headers' basename.
    header_prefix: str

    # Prefix used for C preprocessor macros.
    project_cpp_prefix: str

    # Options to check
    checkers: List[Checker]


class HeaderGenerator:
    """Generate a header to include before or after the user config."""

    def __init__(self, branch_data: BranchData, position: Position) -> None:
        self.branch_data = branch_data
        self.position = position
        self.prefix = branch_data.project_cpp_prefix + '_CONFIG_CHECK'
        self.bypass_checks = self.prefix + '_BYPASS'

    def write_stanza(self, out: typing_util.Writable, checker: Checker) -> None:
        """Write the part of the output corresponding to one config option."""
        code = checker.code(self.position, self.prefix)
        out.write(code)

    def write_content(self, out: typing_util.Writable) -> None:
        """Write the output for all config options to be processed."""
        for checker in self.branch_data.checkers:
            self.write_stanza(out, checker)

    def output_file_name(self) -> str:
        """The base name of the output file."""
        return ''.join([self.branch_data.header_prefix,
                        'config_check_',
                        self.position.name.lower(),
                        '.h'])

    def write(self, directory: str) -> None:
        """Write the whole output file."""
        basename = self.output_file_name()
        with open(os.path.join(directory, basename), 'w') as out:
            out.write(f'''\
/* {basename} (generated part of {self.branch_data.header_prefix}config.c). */
/* Automatically generated by {os.path.basename(sys.argv[0])}. Do not edit! */

#if !defined({self.bypass_checks}) //no-check-names

/* *INDENT-OFF* */
''')
            self.write_content(out)
            out.write(f'''
/* *INDENT-ON* */

#endif /* !defined({self.bypass_checks}) */ //no-check-names

/* End of automatically generated {basename} */
''')


def generate_header_files(branch_data: BranchData,
                          directory: str,
                          list_only: bool = False) -> Iterator[str]:
    """Generate the header files to include before and after *config.h."""
    for position in Position:
        generator = HeaderGenerator(branch_data, position)
        # Make sure to output a path with / even on Windows, so that
        # it can be consumed by tools such as CMake.
        yield posixpath.join(directory, generator.output_file_name())
        if not list_only:
            generator.write(directory)


def main(branch_data: BranchData) -> None:
    root = build_tree.guess_project_root()
    # Is root the current directory? The safe default is no, so compare
    # the paths, rather than calling `os.samefile()` which can have false
    # positives and can fail in edge cases.
    if root == os.getcwd():
        # Be nice and use a relative path when it's simple to do so.
        # (build_tree.guess_project_root() should probably do this, actually.)
        # This is not only nice to humans, but also necessary for
        # `make_generated_files.py --root DIR --check`: it calls
        # this script with `--list` and expects a path that is relative
        # to DIR, not an absolute path that is under the project root.
        root = os.curdir
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--list', action='store_true',
                        help='List generated files and exit')
    parser.add_argument('--list-for-cmake', action='store_true',
                        help='List generated files in CMake-friendly format and exit')
    parser.add_argument('output_directory', metavar='DIR', nargs='?',
                        default=posixpath.join(root, branch_data.header_directory),
                        help='output file location (default: %(default)s)')
    options = parser.parse_args()
    list_only = options.list or options.list_for_cmake
    output_files = list(generate_header_files(branch_data,
                                              options.output_directory,
                                              list_only=list_only))
    if options.list_for_cmake:
        sys.stdout.write(';'.join(output_files))
    elif options.list:
        for filename in output_files:
            print(filename)
