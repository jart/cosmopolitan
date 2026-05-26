"""Common code for test data generation.

This module defines classes that are of general use to automatically
generate .data files for unit tests, as well as a main function.

These are used both by generate_psa_tests.py and generate_bignum_tests.py.
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#

import argparse
import io
import os
import posixpath
import re
import inspect

from abc import ABCMeta, abstractmethod
from typing import Callable, Dict, Iterable, Iterator, List, Type, TypeVar

from . import build_tree
from . import test_case

T = TypeVar('T') #pylint: disable=invalid-name


class BaseTest(metaclass=ABCMeta):
    """Base class for test case generation.

    Attributes:
        count: Counter for test cases from this class.
        case_description: Short description of the test case. This may be
            automatically generated using the class, or manually set.
        dependencies: A list of dependencies required for the test case.
        show_test_count: Toggle for inclusion of `count` in the test description.
        test_function: Test function which the class generates cases for.
        test_name: A common name or description of the test function. This can
            be `test_function`, a clearer equivalent, or a short summary of the
            test function's purpose.
    """
    count = 0
    case_description = ""
    dependencies = [] # type: List[str]
    show_test_count = True
    test_function = ""
    test_name = ""

    def __new__(cls, *args, **kwargs):
        # pylint: disable=unused-argument
        cls.count += 1
        return super().__new__(cls)

    @abstractmethod
    def arguments(self) -> List[str]:
        """Get the list of arguments for the test case.

        Override this method to provide the list of arguments required for
        the `test_function`.

        Returns:
            List of arguments required for the test function.
        """
        raise NotImplementedError

    def description(self) -> str:
        """Create a test case description.

        Creates a description of the test case, including a name for the test
        function, an optional case count, and a description of the specific
        test case. This should inform a reader what is being tested, and
        provide context for the test case.

        Returns:
            Description for the test case.
        """
        if self.show_test_count:
            return "{} #{} {}".format(
                self.test_name, self.count, self.case_description
                ).strip()
        else:
            return "{} {}".format(self.test_name, self.case_description).strip()


    def create_test_case(self) -> test_case.TestCase:
        """Generate TestCase from the instance."""
        tc = test_case.TestCase()
        tc.set_description(self.description())
        tc.set_function(self.test_function)
        tc.set_arguments(self.arguments())
        tc.set_dependencies(self.dependencies)

        return tc

    @classmethod
    @abstractmethod
    def generate_function_tests(cls) -> Iterator[test_case.TestCase]:
        """Generate test cases for the class test function.

        This will be called in classes where `test_function` is set.
        Implementations should yield TestCase objects, by creating instances
        of the class with appropriate input data, and then calling
        `create_test_case()` on each.
        """
        raise NotImplementedError


class BaseTarget:
    #pylint: disable=too-few-public-methods
    """Base target for test case generation.

    Child classes of this class represent an output file, and can be referred
    to as file targets. These indicate where test cases will be written to for
    all subclasses of the file target, which is set by `target_basename`.

    Attributes:
        target_basename: Basename of file to write generated tests to. This
            should be specified in a child class of BaseTarget.
    """
    target_basename = ""

    @classmethod
    def generate_tests(cls) -> Iterator[test_case.TestCase]:
        """Generate test cases for the class and its subclasses.

        In classes with `test_function` set, `generate_function_tests()` is
        called to generate test cases first.

        In all classes, this method will iterate over its subclasses, and
        yield from `generate_tests()` in each. Calling this method on a class X
        will yield test cases from all classes derived from X.
        """
        if issubclass(cls, BaseTest) and not inspect.isabstract(cls):
            #pylint: disable=no-member
            yield from cls.generate_function_tests()
        for subclass in sorted(cls.__subclasses__(), key=lambda c: c.__name__):
            yield from subclass.generate_tests()


class TestGenerator:
    """Generate test cases and write to data files."""

    # Note that targets whose names contain 'test_format' have their content
    # validated by `abi_check.py`.
    targets = {} # type: Dict[str, Callable[..., Iterable[test_case.TestCase]]]

    def __init__(self, options) -> None:
        self.test_suite_directory = options.directory
        # Update `targets` with an entry for each child class of BaseTarget.
        # Each entry represents a file generated by the BaseTarget framework,
        # and enables generating the .data files using the CLI.
        self.targets.update({
            subclass.target_basename: subclass.generate_tests
            for subclass in BaseTarget.__subclasses__()
            if subclass.target_basename
        })

    def filename_for(self, basename: str) -> str:
        """The location of the data file with the specified base name."""
        return posixpath.join(self.test_suite_directory, basename + '.data')

    def write_test_data_file(self, basename: str,
                             test_cases: Iterable[test_case.TestCase]) -> None:
        """Write the test cases to a .data file.

        The output file is ``basename + '.data'`` in the test suite directory.
        """
        filename = self.filename_for(basename)
        test_case.write_data_file(filename, test_cases)

    def generate_target(self, name: str, *target_args) -> None:
        """Generate cases and write to data file for a target.

        For target callables which require arguments, override this function
        and pass these arguments using super() (see PSATestGenerator).
        """
        test_cases = self.targets[name](*target_args)
        self.write_test_data_file(name, test_cases)

    def is_up_to_date(self, target) -> bool:
        """Check if the given target already has the expected content."""
        filename = self.filename_for(target)
        if not os.path.exists(filename):
            return False
        test_cases = self.targets[target]()
        out = io.StringIO()
        test_case.write_data_stream(out, test_cases)
        out.seek(0)
        new_content = out.read()
        out.close()
        with open(filename) as current_file:
            old_content = current_file.read()
        return new_content == old_content


def main(args, description: str, generator_class: Type[TestGenerator] = TestGenerator):
    """Command line entry point."""
    parser = argparse.ArgumentParser(description=description)
    parser.add_argument('--list', action='store_true',
                        help='List available targets and exit')
    parser.add_argument('--list-for-cmake', action='store_true',
                        help='Print \';\'-separated list of available targets and exit')
    parser.add_argument('--list-outdated', action='store_true',
                        help=('List outdated targets and exit '
                              '(succeeds even if there are outdated or missing targets)'))
    # If specified explicitly, this option may be a path relative to the
    # current directory when the script is invoked. The default value
    # is relative to the mbedtls root, which we don't know yet. So we
    # can't set a string as the default value here.
    parser.add_argument('--directory', metavar='DIR',
                        help='Output directory (default: tests/suites)')
    parser.add_argument('targets', nargs='*', metavar='TARGET',
                        help='Target file to generate (default: all; "-": none)')
    options = parser.parse_args(args)

    # Change to the mbedtls root, to keep things simple. But first, adjust
    # command line options that might be relative paths.
    if options.directory is None:
        options.directory = 'tests/suites'
    else:
        options.directory = os.path.abspath(options.directory)
    build_tree.chdir_to_root()

    generator = generator_class(options)
    if options.list:
        for name in sorted(generator.targets):
            print(generator.filename_for(name))
        return
    # List in a cmake list format (i.e. ';'-separated)
    if options.list_for_cmake:
        print(';'.join(generator.filename_for(name)
                       for name in sorted(generator.targets)), end='')
        return
    if options.targets:
        # Allow "-" as a special case so you can run
        # ``generate_xxx_tests.py - $targets`` and it works uniformly whether
        # ``$targets`` is empty or not.
        options.targets = [os.path.basename(re.sub(r'\.data\Z', r'', target))
                           for target in options.targets
                           if target != '-']
    else:
        options.targets = sorted(generator.targets)
    for target in options.targets:
        if options.list_outdated:
            if not generator.is_up_to_date(target):
                print(generator.filename_for(target))
        else:
            generator.generate_target(target)
