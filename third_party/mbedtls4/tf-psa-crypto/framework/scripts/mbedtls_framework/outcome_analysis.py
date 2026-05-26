"""Outcome file analysis code.

This module is the bulk of the code of tests/scripts/analyze_outcomes.py
in each consuming branch. The consuming script is expected to derive
the classes with branch-specific customizations such as ignore lists.
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

import argparse
import gzip
import lzma
import sys
import traceback
import re
import subprocess
import os
import typing

from . import collect_test_cases


# `ComponentOutcomes` is a named tuple which is defined as:
# ComponentOutcomes(
#     successes = {
#         "<suite_case>",
#         ...
#     },
#     failures = {
#         "<suite_case>",
#         ...
#     }
# )
# suite_case = "<suite>;<case>"
ComponentOutcomes = typing.NamedTuple('ComponentOutcomes',
                                      [('successes', typing.Set[str]),
                                       ('failures', typing.Set[str])])

# `Outcomes` is a representation of the outcomes file,
# which defined as:
# Outcomes = {
#     "<component>": ComponentOutcomes,
#     ...
# }
Outcomes = typing.Dict[str, ComponentOutcomes]


class Results:
    """Process analysis results."""

    def __init__(self,
                 stderr: bool = True,
                 log_file: str = '') -> None:
        """Log and count errors.

        Log to stderr if stderr is true.
        Log to log_file if specified and non-empty.
        """
        self.error_count = 0
        self.warning_count = 0
        self.stderr = stderr
        self.log_file = None
        if log_file:
            self.log_file = open(log_file, 'w', encoding='utf-8')

    def new_section(self, fmt, *args, **kwargs):
        self._print_line('\n*** ' + fmt + ' ***\n', *args, **kwargs)

    def info(self, fmt, *args, **kwargs):
        self._print_line('Info: ' + fmt, *args, **kwargs)

    def error(self, fmt, *args, **kwargs):
        self.error_count += 1
        self._print_line('Error: ' + fmt, *args, **kwargs)

    def warning(self, fmt, *args, **kwargs):
        self.warning_count += 1
        self._print_line('Warning: ' + fmt, *args, **kwargs)

    def _print_line(self, fmt, *args, **kwargs):
        line = (fmt + '\n').format(*args, **kwargs)
        if self.stderr:
            sys.stderr.write(line)
        if self.log_file:
            self.log_file.write(line)

def execute_reference_driver_tests(results: Results, ref_component: str, driver_component: str, \
                                   outcome_file: str) -> None:
    """Run the tests specified in ref_component and driver_component. Results
    are stored in the output_file and they will be used for the following
    coverage analysis"""
    results.new_section("Test {} and {}", ref_component, driver_component)

    shell_command = "tests/scripts/all.sh --outcome-file " + outcome_file + \
                    " " + ref_component + " " + driver_component
    results.info("Running: {}", shell_command)
    ret_val = subprocess.run(shell_command.split(), check=False).returncode

    if ret_val != 0:
        results.error("failed to run reference/driver components")

IgnoreEntry = typing.Union[str, typing.Pattern]

def name_matches_pattern(name: str, str_or_re: IgnoreEntry) -> bool:
    """Check if name matches a pattern, that may be a string or regex.
    - If the pattern is a string, name must be equal to match.
    - If the pattern is a regex, name must fully match.
    """
    # The CI's python is too old for re.Pattern
    #if isinstance(str_or_re, re.Pattern):
    if not isinstance(str_or_re, str):
        return str_or_re.fullmatch(name) is not None
    else:
        return str_or_re == name

def open_outcome_file(outcome_file: str) -> typing.TextIO:
    if outcome_file.endswith('.gz'):
        return gzip.open(outcome_file, 'rt', encoding='utf-8')
    elif outcome_file.endswith('.xz'):
        return lzma.open(outcome_file, 'rt', encoding='utf-8')
    else:
        return open(outcome_file, 'rt', encoding='utf-8')

def read_outcome_file(outcome_file: str) -> Outcomes:
    """Parse an outcome file and return an outcome collection.
    """
    outcomes = {}
    with open_outcome_file(outcome_file) as input_file:
        for line in input_file:
            (_platform, component, suite, case, result, _cause) = line.split(';')
            # Note that `component` is not unique. If a test case passes on Linux
            # and fails on FreeBSD, it'll end up in both the successes set and
            # the failures set.
            suite_case = ';'.join([suite, case])
            if component not in outcomes:
                outcomes[component] = ComponentOutcomes(set(), set())
            if result == 'PASS':
                outcomes[component].successes.add(suite_case)
            elif result == 'FAIL':
                outcomes[component].failures.add(suite_case)

    return outcomes


class Task:
    """Base class for outcome analysis tasks."""

    # Override the following in child classes.
    # Map test suite names (with the test_suite_prefix) to a list of ignored
    # test cases. Each element in the list can be either a string or a regex;
    # see the `name_matches_pattern` function.
    IGNORED_TESTS = {} #type: typing.Dict[str, typing.List[IgnoreEntry]]

    def __init__(self, options) -> None:
        """Pass command line options to the tasks.

        Each task decides which command line options it cares about.
        """
        pass

    def section_name(self) -> str:
        """The section name to use in results."""
        raise NotImplementedError

    def ignored_tests(self, test_suite: str) -> typing.Iterator[IgnoreEntry]:
        """Generate the ignore list for the specified test suite."""
        if test_suite in self.IGNORED_TESTS:
            yield from self.IGNORED_TESTS[test_suite]
        pos = test_suite.find('.')
        if pos != -1:
            base_test_suite = test_suite[:pos]
            if base_test_suite in self.IGNORED_TESTS:
                yield from self.IGNORED_TESTS[base_test_suite]

    def is_test_case_ignored(self, test_suite: str, test_string: str) -> bool:
        """Check if the specified test case is ignored."""
        for str_or_re in self.ignored_tests(test_suite):
            if name_matches_pattern(test_string, str_or_re):
                return True
        return False

    def run(self, results: Results, outcomes: Outcomes):
        """Run the analysis on the specified outcomes.

        Signal errors via the results objects
        """
        raise NotImplementedError


class CoverageTask(Task):
    """Analyze test coverage."""

    # Test cases whose suite and description are matched by an entry in
    # IGNORED_TESTS are expected to be never executed.
    # All other test cases are expected to be executed at least once.

    def __init__(self, options) -> None:
        super().__init__(options)
        self.full_coverage = options.full_coverage #type: bool

    @staticmethod
    def section_name() -> str:
        return "Analyze coverage"

    def run(self, results: Results, outcomes: Outcomes) -> None:
        """Check that all available test cases are executed at least once."""
        # Make sure that the generated data files are present (and up-to-date).
        # This allows analyze_outcomes.py to run correctly on a fresh Git
        # checkout.
        cp = subprocess.run(['make', 'generated_files'],
                            cwd='tests',
                            stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                            check=False)
        if cp.returncode != 0:
            sys.stderr.write(cp.stdout.decode('utf-8'))
            results.error("Failed \"make generated_files\" in tests. "
                          "Coverage analysis may be incorrect.")
        available = collect_test_cases.collect_available_test_cases()
        for suite_case in available:
            hit = any(suite_case in comp_outcomes.successes or
                      suite_case in comp_outcomes.failures
                      for comp_outcomes in outcomes.values())
            (test_suite, test_description) = suite_case.split(';')
            ignored = self.is_test_case_ignored(test_suite, test_description)

            if not hit and not ignored:
                if self.full_coverage:
                    results.error('Test case not executed: {}', suite_case)
                else:
                    results.warning('Test case not executed: {}', suite_case)
            elif hit and ignored:
                # If a test case is no longer always skipped, we should remove
                # it from the ignore list.
                if self.full_coverage:
                    results.error('Test case was executed but marked as ignored for coverage: {}',
                                  suite_case)
                else:
                    results.warning('Test case was executed but marked as ignored for coverage: {}',
                                    suite_case)


class DriverVSReference(Task):
    """Compare outcomes from testing with and without a driver.

    There are 2 options to use analyze_driver_vs_reference_xxx locally:
    1. Run tests and then analysis:
      - tests/scripts/all.sh --outcome-file "$PWD/out.csv" <component_ref> <component_driver>
      - tests/scripts/analyze_outcomes.py out.csv analyze_driver_vs_reference_xxx
    2. Let this script run both automatically:
      - tests/scripts/analyze_outcomes.py out.csv analyze_driver_vs_reference_xxx
    """

    # Override the following in child classes.
    # Configuration name (all.sh component) used as the reference.
    REFERENCE = ''
    # Configuration name (all.sh component) used as the driver.
    DRIVER = ''
    # Ignored test suites (without the test_suite_ prefix).
    IGNORED_SUITES = [] #type: typing.List[str]

    def __init__(self, options) -> None:
        super().__init__(options)
        self.ignored_suites = frozenset('test_suite_' + x
                                        for x in self.IGNORED_SUITES)

    def section_name(self) -> str:
        return f"Analyze driver {self.DRIVER} vs reference {self.REFERENCE}"

    def run(self, results: Results, outcomes: Outcomes) -> None:
        """Check that all tests passing in the driver component are also
        passing in the corresponding reference component.
        Skip:
        - full test suites provided in ignored_suites list
        - only some specific test inside a test suite, for which the corresponding
          output string is provided
        """
        ref_outcomes = outcomes.get("component_" + self.REFERENCE)
        driver_outcomes = outcomes.get("component_" + self.DRIVER)

        if ref_outcomes is None or driver_outcomes is None:
            results.error("required components are missing: bad outcome file?")
            return

        if not ref_outcomes.successes:
            results.error("no passing test in reference component: bad outcome file?")
            return

        for suite_case in ref_outcomes.successes:
            # suite_case is like "test_suite_foo.bar;Description of test case"
            (full_test_suite, test_string) = suite_case.split(';')
            test_suite = full_test_suite.split('.')[0] # retrieve main part of test suite name

            # Immediately skip fully-ignored test suites
            if test_suite in self.ignored_suites or \
               full_test_suite in self.ignored_suites:
                continue

            # For ignored test cases inside test suites, just remember and:
            # don't issue an error if they're skipped with drivers,
            # but issue an error if they're not (means we have a bad entry).
            ignored = self.is_test_case_ignored(full_test_suite, test_string)

            if not ignored and not suite_case in driver_outcomes.successes:
                results.error("SKIP/FAIL -> PASS: {}", suite_case)
            if ignored and suite_case in driver_outcomes.successes:
                results.error("uselessly ignored: {}", suite_case)


# Set this to False if a consuming branch can't achieve full test coverage
# in its default CI run.
FULL_COVERAGE_BY_DEFAULT = True

def main(known_tasks: typing.Dict[str, typing.Type[Task]]) -> None:
    try:
        parser = argparse.ArgumentParser(description=__doc__)
        parser.add_argument('outcomes', metavar='OUTCOMES.CSV',
                            help='Outcome file to analyze (can be .gz or .xz)')
        parser.add_argument('specified_tasks', default='all', nargs='?',
                            help='Analysis to be done. By default, run all tasks. '
                                 'With one or more TASK, run only those. '
                                 'TASK can be the name of a single task or '
                                 'comma/space-separated list of tasks. ')
        parser.add_argument('--allow-partial-coverage', action='store_false',
                            dest='full_coverage', default=FULL_COVERAGE_BY_DEFAULT,
                            help=("Only warn if a test case is skipped in all components" +
                                  (" (default)" if not FULL_COVERAGE_BY_DEFAULT else "") +
                                  ". Only used by the 'analyze_coverage' task."))
        parser.add_argument('--list', action='store_true',
                            help='List all available tasks and exit.')
        parser.add_argument('--log-file',
                            default='tests/analyze_outcomes.log',
                            help='Log file (default: tests/analyze_outcomes.log;'
                                 ' empty means no log file)')
        parser.add_argument('--require-full-coverage', action='store_true',
                            dest='full_coverage', default=FULL_COVERAGE_BY_DEFAULT,
                            help=("Require all available test cases to be executed" +
                                  (" (default)" if FULL_COVERAGE_BY_DEFAULT else "") +
                                  ". Only used by the 'analyze_coverage' task."))
        options = parser.parse_args()

        if options.list:
            for task_name in known_tasks:
                print(task_name)
            sys.exit(0)

        main_results = Results(log_file=options.log_file)

        if options.specified_tasks == 'all':
            tasks_list = list(known_tasks.keys())
        else:
            tasks_list = re.split(r'[, ]+', options.specified_tasks)
            for task_name in tasks_list:
                if task_name not in known_tasks:
                    sys.stderr.write('invalid task: {}\n'.format(task_name))
                    sys.exit(2)

        # If the outcome file exists, parse it once and share the result
        # among tasks to improve performance.
        # Otherwise, it will be generated by execute_reference_driver_tests.
        if not os.path.exists(options.outcomes):
            if len(tasks_list) > 1:
                sys.stderr.write("mutiple tasks found, please provide a valid outcomes file.\n")
                sys.exit(2)

            task_name = tasks_list[0]
            task_class = known_tasks[task_name]
            if not issubclass(task_class, DriverVSReference):
                sys.stderr.write("please provide valid outcomes file for {}.\n".format(task_name))
                sys.exit(2)
            # mypy isn't smart enough to know that REFERENCE and DRIVER
            # are *class* attributes of all classes derived from
            # DriverVSReference. (It would be smart enough if we had an
            # instance of task_class, but we can't construct an instance
            # until we have the outcome data, so at this point we only
            # have the class.) So we use indirection to access the class
            # attributes.
            execute_reference_driver_tests(main_results,
                                           getattr(task_class, 'REFERENCE'),
                                           getattr(task_class, 'DRIVER'),
                                           options.outcomes)

        outcomes = read_outcome_file(options.outcomes)

        for task_name in tasks_list:
            task_constructor = known_tasks[task_name]
            task_instance = task_constructor(options)
            main_results.new_section(task_instance.section_name())
            task_instance.run(main_results, outcomes)

        main_results.info("Overall results: {} warnings and {} errors",
                          main_results.warning_count, main_results.error_count)

        sys.exit(0 if (main_results.error_count == 0) else 1)

    except Exception: # pylint: disable=broad-except
        # Print the backtrace and exit explicitly with our chosen status.
        traceback.print_exc()
        sys.exit(120)
