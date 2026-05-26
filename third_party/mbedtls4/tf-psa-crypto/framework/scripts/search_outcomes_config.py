#!/usr/bin/env python3
"""Search an outcome file for configurations with given settings.

Read an outcome file and report the configurations in which test_suite_config
runs with the required settings (compilation option enabled or disabled).
"""

import argparse
import os
import re
import subprocess
from typing import Dict, FrozenSet, Iterator, List, Set
import tempfile
import unittest

from mbedtls_framework import build_tree


def make_regexp_for_settings(settings: List[str]) -> str:
    """Construct a regexp matching the interesting outcome lines.

    Interesting outcome lines are from test_suite_config where the given
    setting is passing.

    We assume that the elements of settings don't contain regexp special
    characters.
    """
    return (r';test_suite_config[^;]*;Config: (' +
            '|'.join(settings) +
            r');PASS;')

def run_grep(regexp: str, outcome_file: str) -> List[str]:
    """Run grep on the outcome file and return the matching lines."""
    env = os.environ.copy()
    env['LC_ALL'] = 'C' # Speeds up some versions of GNU grep
    try:
        return subprocess.check_output(['grep', '-E', regexp, outcome_file],
                                       encoding='ascii',
                                       env=env).splitlines()
    except subprocess.CalledProcessError as exn:
        if exn.returncode == 1:
            return [] # No results. We don't consider this an error.
        raise

OUTCOME_LINE_RE = re.compile(r'[^;]*;'
                             r'([^;]*);'
                             r'test_suite_config\.(?:[^;]*);'
                             r'Config: ([^;]*);'
                             r'PASS;')

def extract_configuration_data(outcome_lines: List[str]) -> Dict[str, FrozenSet[str]]:
    """Extract the configuration data from outcome lines.

    The result maps a configuration name to the list of passing settings
    in that configuration.
    """
    config_data = {} #type: Dict[str, Set[str]]
    for line in outcome_lines:
        m = OUTCOME_LINE_RE.match(line)
        # Assuming a well-formed outcome file, make_regexp_for_settings()
        # arranges to only return lines that should match OUTCOME_LINE_RE.
        # So this assertion can't fail unless there is an unexpected
        # divergence between OUTCOME_LINE_RE, make_regexp_for_settings()
        # and the format of the given outcome file
        assert m is not None
        config_name, setting = m.groups()
        if config_name not in config_data:
            config_data[config_name] = set()
        config_data[config_name].add(setting)
    return dict((name, frozenset(settings))
                for name, settings in config_data.items())


def matching_configurations(config_data: Dict[str, FrozenSet[str]],
                            required: List[str]) -> Iterator[str]:
    """Search configurations with the given passing settings.

    config_data maps a configuration name to the list of passing settings
    in that configuration.

    Each setting should be an Mbed TLS compile setting (MBEDTLS_xxx or
    PSA_xxx), optionally prefixed with "!".
    """
    required_set = frozenset(required)
    for config, observed in config_data.items():
        if required_set.issubset(observed):
            yield config

def search_config_outcomes(outcome_file: str, settings: List[str]) -> List[str]:
    """Search the given outcome file for reports of the given settings.

    Each setting should be an Mbed TLS compile setting (MBEDTLS_xxx or
    PSA_xxx), optionally prefixed with "!".
    """
    # The outcome file is large enough (hundreds of MB) that parsing it
    # in Python is slow. Use grep to speed this up considerably.
    regexp = make_regexp_for_settings(settings)
    outcome_lines = run_grep(regexp, outcome_file)
    config_data = extract_configuration_data(outcome_lines)
    return sorted(matching_configurations(config_data, settings))


class TestSearch(unittest.TestCase):
    """Tests of search functionality."""

    OUTCOME_FILE_CONTENT = """\
whatever;foobar;test_suite_config.part;Config: MBEDTLS_FOO;PASS;
whatever;foobar;test_suite_config.part;Config: !MBEDTLS_FOO;SKIP;
whatever;foobar;test_suite_config.part;Config: MBEDTLS_BAR;PASS;
whatever;foobar;test_suite_config.part;Config: !MBEDTLS_BAR;SKIP;
whatever;foobar;test_suite_config.part;Config: MBEDTLS_QUX;SKIP;
whatever;foobar;test_suite_config.part;Config: !MBEDTLS_QUX;PASS;
whatever;fooqux;test_suite_config.part;Config: MBEDTLS_FOO;PASS;
whatever;fooqux;test_suite_config.part;Config: !MBEDTLS_FOO;SKIP;
whatever;fooqux;test_suite_config.part;Config: MBEDTLS_BAR;SKIP;
whatever;fooqux;test_suite_config.part;Config: !MBEDTLS_BAR;PASS;
whatever;fooqux;test_suite_config.part;Config: MBEDTLS_QUX;PASS;
whatever;fooqux;test_suite_config.part;Config: !MBEDTLS_QUX;SKIP;
whatever;fooqux;test_suite_something.else;Config: MBEDTLS_BAR;PASS;
whatever;boring;test_suite_config.part;Config: BORING;PASS;
whatever;parasite;not_test_suite_config.not;Config: MBEDTLS_FOO;PASS;
whatever;parasite;test_suite_config.but;Config: MBEDTLS_QUX with bells on;PASS;
whatever;parasite;test_suite_config.but;Not Config: MBEDTLS_QUX;PASS;
"""

    def search(self, settings: List[str], expected: List[str]) -> None:
        """Test the search functionality.

        * settings: settings to search.
        * expected: expected search results.
        """
        with tempfile.NamedTemporaryFile() as tmp:
            tmp.write(self.OUTCOME_FILE_CONTENT.encode())
            tmp.flush()
            actual = search_config_outcomes(tmp.name, settings)
            self.assertEqual(actual, expected)

    def test_foo(self) -> None:
        self.search(['MBEDTLS_FOO'], ['foobar', 'fooqux'])

    def test_bar(self) -> None:
        self.search(['MBEDTLS_BAR'], ['foobar'])

    def test_foo_bar(self) -> None:
        self.search(['MBEDTLS_FOO', 'MBEDTLS_BAR'], ['foobar'])

    def test_foo_notbar(self) -> None:
        self.search(['MBEDTLS_FOO', '!MBEDTLS_BAR'], ['fooqux'])


class TestOutcome(unittest.TestCase):
    """Tests of outcome file format expectations.

    This class builds and runs the config tests in the current configuration.
    The configuration must have at least one feature enabled and at least
    one feature disabled in each category: MBEDTLS_xxx and PSA_WANT_xxx.
    It needs a C compiler.
    """

    outcome_content = '' # Let mypy know this field can be used in test case methods

    @classmethod
    def setUpClass(cls) -> None:
        """Generate, build and run the config tests."""
        root_dir = build_tree.guess_project_root()
        tests_dir = os.path.join(root_dir, 'tests')
        suites = ['test_suite_config.mbedtls_boolean',
                  'test_suite_config.psa_boolean']
        _output = subprocess.check_output(['make'] + suites,
                                          cwd=tests_dir,
                                          stderr=subprocess.STDOUT)
        with tempfile.NamedTemporaryFile(dir=tests_dir) as outcome_file:
            env = os.environ.copy()
            env['MBEDTLS_TEST_PLATFORM'] = 'some_platform'
            env['MBEDTLS_TEST_CONFIGURATION'] = 'some_configuration'
            env['MBEDTLS_TEST_OUTCOME_FILE'] = outcome_file.name
            for suite in suites:
                _output = subprocess.check_output([os.path.join(os.path.curdir, suite)],
                                                  cwd=tests_dir,
                                                  env=env,
                                                  stderr=subprocess.STDOUT)
            cls.outcome_content = outcome_file.read().decode('ascii')

    def test_outcome_format(self) -> None:
        """Check that there are outcome lines matching the expected general format."""
        def regex(prefix: str, result: str) -> str:
            return (r'(?:\A|\n)some_platform;some_configuration;'
                    r'test_suite_config\.\w+;Config: {}_\w+;{};'
                    .format(prefix, result))
        self.assertRegex(self.outcome_content, regex('MBEDTLS', 'PASS'))
        self.assertRegex(self.outcome_content, regex('MBEDTLS', 'SKIP'))
        self.assertRegex(self.outcome_content, regex('!MBEDTLS', 'PASS'))
        self.assertRegex(self.outcome_content, regex('!MBEDTLS', 'SKIP'))
        self.assertRegex(self.outcome_content, regex('PSA_WANT', 'PASS'))
        self.assertRegex(self.outcome_content, regex('PSA_WANT', 'SKIP'))
        self.assertRegex(self.outcome_content, regex('!PSA_WANT', 'PASS'))
        self.assertRegex(self.outcome_content, regex('!PSA_WANT', 'SKIP'))

    def test_outcome_lines(self) -> None:
        """Look for some sample outcome lines."""
        def regex(setting: str) -> str:
            return (r'(?:\A|\n)some_platform;some_configuration;'
                    r'test_suite_config\.\w+;Config: {};(PASS|SKIP);'
                    .format(setting))
        self.assertRegex(self.outcome_content, regex('MBEDTLS_AES_C'))
        self.assertRegex(self.outcome_content, regex('MBEDTLS_AES_ROM_TABLES'))
        self.assertRegex(self.outcome_content, regex('MBEDTLS_SSL_CLI_C'))
        self.assertRegex(self.outcome_content, regex('MBEDTLS_X509_CRT_PARSE_C'))
        self.assertRegex(self.outcome_content, regex('PSA_WANT_ALG_HMAC'))
        self.assertRegex(self.outcome_content, regex('PSA_WANT_KEY_TYPE_AES'))

def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--outcome-file', '-f', metavar='FILE',
                        default='outcomes.csv',
                        help='Outcome file to read (default: outcomes.csv)')
    parser.add_argument('settings', metavar='SETTING', nargs='+',
                        help='Required setting (e.g. "MBEDTLS_RSA_C" or "!PSA_WANT_ALG_SHA256")')
    options = parser.parse_args()
    found = search_config_outcomes(options.outcome_file, options.settings)
    for name in found:
        print(name)

if __name__ == '__main__':
    main()
