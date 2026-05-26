#!/usr/bin/env python3

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

"""
This script checks the current state of the source code for minor issues,
including incorrect file permissions, presence of tabs, non-Unix line endings,
trailing whitespace, and presence of UTF-8 BOM.
Note: requires python 3, must be run from Mbed TLS root.
"""

import argparse
import codecs
import inspect
import logging
import os
import re
import subprocess
import sys
try:
    from typing import FrozenSet, Optional, Pattern # pylint: disable=unused-import
except ImportError:
    pass

from mbedtls_framework import build_tree


class FileIssueTracker:
    """Base class for file-wide issue tracking.

    To implement a checker that processes a file as a whole, inherit from
    this class and implement `check_file_for_issue` and define ``heading``.

    ``suffix_exemptions``: files whose name ends with a string in this set
     will not be checked.

    ``path_exemptions``: files whose path (relative to the root of the source
    tree) matches this regular expression will not be checked. This can be
    ``None`` to match no path. Paths are normalized and converted to ``/``
    separators before matching.

    ``heading``: human-readable description of the issue
    """

    suffix_exemptions = frozenset() #type: FrozenSet[str]
    path_exemptions = None #type: Optional[Pattern[str]]
    # heading must be defined in derived classes.
    # pylint: disable=no-member

    def __init__(self):
        self.files_with_issues = {}

    @staticmethod
    def normalize_path(filepath):
        """Normalize ``filepath`` with / as the directory separator."""
        filepath = os.path.normpath(filepath)
        # On Windows, we may have backslashes to separate directories.
        # We need slashes to match exemption lists.
        seps = os.path.sep
        if os.path.altsep is not None:
            seps += os.path.altsep
        return '/'.join(filepath.split(seps))

    def should_check_file(self, filepath):
        """Whether the given file name should be checked.

        Files whose name ends with a string listed in ``self.suffix_exemptions``
        or whose path matches ``self.path_exemptions`` will not be checked.
        """
        for files_exemption in self.suffix_exemptions:
            if filepath.endswith(files_exemption):
                return False
        if self.path_exemptions and \
           re.match(self.path_exemptions, self.normalize_path(filepath)):
            return False
        return True

    def check_file_for_issue(self, filepath):
        """Check the specified file for the issue that this class is for.

        Subclasses must implement this method.
        """
        raise NotImplementedError

    def record_issue(self, filepath, line_number):
        """Record that an issue was found at the specified location."""
        if filepath not in self.files_with_issues.keys():
            self.files_with_issues[filepath] = []
        self.files_with_issues[filepath].append(line_number)

    def output_file_issues(self, logger):
        """Log all the locations where the issue was found."""
        if self.files_with_issues.values():
            logger.info(self.heading)
            for filename, lines in sorted(self.files_with_issues.items()):
                if lines:
                    logger.info("{}: {}".format(
                        filename, ", ".join(str(x) for x in lines)
                    ))
                else:
                    logger.info(filename)
            logger.info("")

BINARY_FILE_PATH_RE_LIST = [
    r'docs/.*\.pdf\Z',
    r'docs/.*\.png\Z',
    r'tf-psa-crypto/docs/.*\.pdf\Z',
    r'tf-psa-crypto/docs/.*\.png\Z',
    r'programs/fuzz/corpuses/[^.]+\Z',
    r'framework/data_files/[^.]+\Z',
    r'framework/data_files/.*\.(crt|csr|db|der|key|pubkey)\Z',
    r'framework/data_files/.*\.req\.[^/]+\Z',
    r'framework/data_files/.*malformed[^/]+\Z',
    r'framework/data_files/format_pkcs12\.fmt\Z',
    r'framework/data_files/.*\.bin\Z',
]
BINARY_FILE_PATH_RE = re.compile('|'.join(BINARY_FILE_PATH_RE_LIST))

class LineIssueTracker(FileIssueTracker):
    """Base class for line-by-line issue tracking.

    To implement a checker that processes files line by line, inherit from
    this class and implement `line_with_issue`.
    """

    # Exclude binary files.
    path_exemptions = BINARY_FILE_PATH_RE

    def issue_with_line(self, line, filepath, line_number):
        """Check the specified line for the issue that this class is for.

        Subclasses must implement this method.
        """
        raise NotImplementedError

    def check_file_line(self, filepath, line, line_number):
        if self.issue_with_line(line, filepath, line_number):
            self.record_issue(filepath, line_number)

    def check_file_for_issue(self, filepath):
        """Check the lines of the specified file.

        Subclasses must implement the ``issue_with_line`` method.
        """
        with open(filepath, "rb") as f:
            for i, line in enumerate(iter(f.readline, b"")):
                self.check_file_line(filepath, line, i + 1)


def is_windows_file(filepath):
    _root, ext = os.path.splitext(filepath)
    return ext in ('.bat', '.dsp', '.dsw', '.sln', '.vcxproj')


class ShebangIssueTracker(FileIssueTracker):
    """Track files with a bad, missing or extraneous shebang line.

    Executable scripts must start with a valid shebang (#!) line.
    """

    heading = "Invalid shebang line:"

    # Allow either /bin/sh, /bin/bash, or /usr/bin/env.
    # Allow at most one argument (this is a Linux limitation).
    # For sh and bash, the argument if present must be options.
    # For env, the argument must be the base name of the interpreter.
    _shebang_re = re.compile(rb'^#! ?(?:/bin/(bash|sh)(?: -[^\n ]*)?'
                             rb'|/usr/bin/env ([^\n /]+))$')
    _extensions = {
        b'bash': 'sh',
        b'perl': 'pl',
        b'python3': 'py',
        b'sh': 'sh',
    }

    path_exemptions = re.compile(r'framework/scripts/quiet/.*')

    def is_valid_shebang(self, first_line, filepath):
        m = re.match(self._shebang_re, first_line)
        if not m:
            return False
        interpreter = m.group(1) or m.group(2)
        if interpreter not in self._extensions:
            return False
        if not filepath.endswith('.' + self._extensions[interpreter]):
            return False
        return True

    def check_file_for_issue(self, filepath):
        is_executable = os.access(filepath, os.X_OK)
        with open(filepath, "rb") as f:
            first_line = f.readline()
        if first_line.startswith(b'#!'):
            if not is_executable:
                # Shebang on a non-executable file
                self.files_with_issues[filepath] = None
            elif not self.is_valid_shebang(first_line, filepath):
                self.files_with_issues[filepath] = [1]
        elif is_executable:
            # Executable without a shebang
            self.files_with_issues[filepath] = None


class EndOfFileNewlineIssueTracker(FileIssueTracker):
    """Track files that end with an incomplete line
    (no newline character at the end of the last line)."""

    heading = "Missing newline at end of file:"

    path_exemptions = BINARY_FILE_PATH_RE

    def check_file_for_issue(self, filepath):
        with open(filepath, "rb") as f:
            try:
                f.seek(-1, 2)
            except OSError:
                # This script only works on regular files. If we can't seek
                # 1 before the end, it means that this position is before
                # the beginning of the file, i.e. that the file is empty.
                return
            if f.read(1) != b"\n":
                self.files_with_issues[filepath] = None


class Utf8BomIssueTracker(FileIssueTracker):
    """Track files that start with a UTF-8 BOM.
    Files should be ASCII or UTF-8. Valid UTF-8 does not start with a BOM."""

    heading = "UTF-8 BOM present:"

    suffix_exemptions = frozenset([".vcxproj", ".sln"])
    path_exemptions = BINARY_FILE_PATH_RE

    def check_file_for_issue(self, filepath):
        with open(filepath, "rb") as f:
            if f.read().startswith(codecs.BOM_UTF8):
                self.files_with_issues[filepath] = None


class UnicodeIssueTracker(LineIssueTracker):
    """Track lines with invalid characters or invalid text encoding."""

    heading = "Invalid UTF-8 or forbidden character:"

    # Only allow valid UTF-8, and only other explicitly allowed characters.
    # We deliberately exclude all characters that aren't a simple non-blank,
    # non-zero-width glyph, apart from a very small set (tab, ordinary space,
    # line breaks, "basic" no-break space and soft hyphen). In particular,
    # non-ASCII control characters, combinig characters, and Unicode state
    # changes (e.g. right-to-left text) are forbidden.
    # Note that we do allow some characters with a risk of visual confusion,
    # for example '-' (U+002D HYPHEN-MINUS) vs '­' (U+00AD SOFT HYPHEN) vs
    # '‐' (U+2010 HYPHEN), or 'A' (U+0041 LATIN CAPITAL LETTER A) vs
    # 'Α' (U+0391 GREEK CAPITAL LETTER ALPHA).
    GOOD_CHARACTERS = ''.join([
        '\t\n\r -~', # ASCII (tabs and line endings are checked separately)
        '\u00A0-\u00FF', # Latin-1 Supplement (for NO-BREAK SPACE and punctuation)
        '\u2010-\u2027\u2030-\u205E', # General Punctuation (printable)
        '\u2070\u2071\u2074-\u208E\u2090-\u209C', # Superscripts and Subscripts
        '\u2190-\u21FF', # Arrows
        '\u2200-\u22FF', # Mathematical Symbols
        '\u2500-\u257F' # Box Drawings characters used in markdown trees
    ])
    # Allow any of the characters and ranges above, and anything classified
    # as a word constituent.
    GOOD_CHARACTERS_RE = re.compile(r'[\w{}]+\Z'.format(GOOD_CHARACTERS))

    def issue_with_line(self, line, _filepath, line_number):
        try:
            text = line.decode('utf-8')
        except UnicodeDecodeError:
            return True
        if line_number == 1 and text.startswith('\uFEFF'):
            # Strip BOM (U+FEFF ZERO WIDTH NO-BREAK SPACE) at the beginning.
            # Which files are allowed to have a BOM is handled in
            # Utf8BomIssueTracker.
            text = text[1:]
        return not self.GOOD_CHARACTERS_RE.match(text)

class UnixLineEndingIssueTracker(LineIssueTracker):
    """Track files with non-Unix line endings (i.e. files with CR)."""

    heading = "Non-Unix line endings:"

    def should_check_file(self, filepath):
        if not super().should_check_file(filepath):
            return False
        return not is_windows_file(filepath)

    def issue_with_line(self, line, _filepath, _line_number):
        return b"\r" in line


class WindowsLineEndingIssueTracker(LineIssueTracker):
    """Track files with non-Windows line endings (i.e. CR or LF not in CRLF)."""

    heading = "Non-Windows line endings:"

    def should_check_file(self, filepath):
        if not super().should_check_file(filepath):
            return False
        return is_windows_file(filepath)

    def issue_with_line(self, line, _filepath, _line_number):
        return not line.endswith(b"\r\n") or b"\r" in line[:-2]


class TrailingWhitespaceIssueTracker(LineIssueTracker):
    """Track lines with trailing whitespace."""

    heading = "Trailing whitespace:"
    suffix_exemptions = frozenset([".diff", ".dsp", ".md", ".patch"])

    def issue_with_line(self, line, _filepath, _line_number):
        return line.rstrip(b"\r\n") != line.rstrip()


class TabIssueTracker(LineIssueTracker):
    """Track lines with tabs."""

    heading = "Tabs present:"
    suffix_exemptions = frozenset([
        ".make",
        ".pem", # some openssl dumps have tabs
        ".sln",
        "/.gitmodules",
        "/Makefile",
        "/Makefile.inc",
        "/generate_visualc_files.pl",
    ])

    def issue_with_line(self, line, _filepath, _line_number):
        return b"\t" in line


class MergeArtifactIssueTracker(LineIssueTracker):
    """Track lines with merge artifacts.
    These are leftovers from a ``git merge`` that wasn't fully edited."""

    heading = "Merge artifact:"

    def issue_with_line(self, line, _filepath, _line_number):
        # Detect leftover git conflict markers.
        if line.startswith(b'<<<<<<< ') or line.startswith(b'>>>>>>> '):
            return True
        if line.startswith(b'||||||| '): # from merge.conflictStyle=diff3
            return True
        if line.rstrip(b'\r\n') == b'=======' and \
           not _filepath.endswith('.md'):
            return True
        return False


def this_location():
    frame = inspect.currentframe()
    assert frame is not None
    info = inspect.getframeinfo(frame)
    return os.path.basename(info.filename), info.lineno
THIS_FILE_BASE_NAME, LINE_NUMBER_BEFORE_LICENSE_ISSUE_TRACKER = this_location()

class LicenseIssueTracker(LineIssueTracker):
    """Check copyright statements and license indications.

    This class only checks that statements are correct if present. It does
    not enforce the presence of statements in each file.
    """

    heading = "License issue:"

    LICENSE_EXEMPTION_RE_LIST = []

    # Exempt third-party drivers which may be under a different license
    if build_tree.looks_like_tf_psa_crypto_root(os.getcwd()):
        LICENSE_EXEMPTION_RE_LIST.append(r'drivers/(?=(everest)/.*)')
    elif build_tree.is_mbedtls_3_6():
        LICENSE_EXEMPTION_RE_LIST.append(r'3rdparty/(?!(p256-m)/.*)')

    LICENSE_EXEMPTION_RE_LIST += [
        # Documentation explaining the license may have accidental
        # false positives.
        r'(ChangeLog|LICENSE|framework\/LICENSE|[-0-9A-Z_a-z]+\.md)\Z',
        # Files imported from TF-M, and not used except in test builds,
        # may be under a different license.
        r'configs/ext/crypto_config_profile_medium\.h\Z',
        r'configs/ext/tfm_mbedcrypto_config_profile_medium\.h\Z',
        r'configs/ext/README\.md\Z',
        # Third-party file.
        r'dco\.txt\Z',
        r'framework\/dco\.txt\Z',
    ]
    path_exemptions = re.compile('|'.join(BINARY_FILE_PATH_RE_LIST +
                                          LICENSE_EXEMPTION_RE_LIST))

    COPYRIGHT_HOLDER = rb'The Mbed TLS Contributors'
    # Catch "Copyright foo", "Copyright (C) foo", "Copyright © foo", etc.
    COPYRIGHT_RE = re.compile(rb'.*\bcopyright\s+((?:\w|\s|[()]|[^ -~])*\w)', re.I)

    SPDX_HEADER_KEY = b'SPDX-License-Identifier'
    LICENSE_IDENTIFIER = b'Apache-2.0 OR GPL-2.0-or-later'
    SPDX_RE = re.compile(br'.*?(' +
                         re.escape(SPDX_HEADER_KEY) +
                         br')(:\s*(.*?)\W*\Z|.*)', re.I)

    LICENSE_MENTION_RE = re.compile(rb'.*(?:' + rb'|'.join([
        rb'Apache License',
        rb'General Public License',
    ]) + rb')', re.I)

    def __init__(self):
        super().__init__()
        # Record what problem was caused. We can't easily report it due to
        # the structure of the script. To be fixed after
        # https://github.com/Mbed-TLS/mbedtls/pull/2506
        self.problem = None

    def issue_with_line(self, line, filepath, line_number):
        #pylint: disable=too-many-return-statements

        # Use endswith() rather than the more correct os.path.basename()
        # because experimentally, it makes a significant difference to
        # the running time.
        if filepath.endswith(THIS_FILE_BASE_NAME) and \
           line_number > LINE_NUMBER_BEFORE_LICENSE_ISSUE_TRACKER:
            # Avoid false positives from the code in this class.
            # Also skip the rest of this file, which is highly unlikely to
            # contain any problematic statements since we put those near the
            # top of files.
            return False

        m = self.COPYRIGHT_RE.match(line)
        if m and m.group(1) != self.COPYRIGHT_HOLDER:
            self.problem = 'Invalid copyright line'
            return True

        m = self.SPDX_RE.match(line)
        if m:
            if m.group(1) != self.SPDX_HEADER_KEY:
                self.problem = 'Misspelled ' + self.SPDX_HEADER_KEY.decode()
                return True
            if not m.group(3):
                self.problem = 'Improperly formatted SPDX license identifier'
                return True
            if m.group(3) != self.LICENSE_IDENTIFIER:
                self.problem = 'Wrong SPDX license identifier'
                return True

        m = self.LICENSE_MENTION_RE.match(line)
        if m:
            self.problem = 'Suspicious license mention'
            return True

        return False


class ErrorAddIssueTracker(LineIssueTracker):
    """Signal direct additions of error codes.

    Adding a low-level error code with a high-level error code is deprecated
    and should use MBEDTLS_ERROR_ADD.
    """

    heading = "Direct addition of error codes"

    _ERR_PLUS_RE = re.compile(br'MBEDTLS_ERR_\w+ *\+|'
                              br'\+ *MBEDTLS_ERR_')
    _EXCLUDE_RE = re.compile(br' *case ')

    def issue_with_line(self, line, filepath, line_number):
        if self._ERR_PLUS_RE.search(line) and not self._EXCLUDE_RE.match(line):
            return True
        return False


class IntegrityChecker:
    """Sanity-check files under the current directory."""

    def __init__(self, log_file):
        """Instantiate the sanity checker.
        Check files under the current directory.
        Write a report of issues to log_file."""
        if not build_tree.looks_like_root(os.getcwd()):
            raise Exception("This script must be run from Mbed TLS or TF-PSA-Crypto root")
        self.logger = None
        self.setup_logger(log_file)
        self.issues_to_check = [
            ShebangIssueTracker(),
            EndOfFileNewlineIssueTracker(),
            Utf8BomIssueTracker(),
            UnicodeIssueTracker(),
            UnixLineEndingIssueTracker(),
            WindowsLineEndingIssueTracker(),
            TrailingWhitespaceIssueTracker(),
            TabIssueTracker(),
            MergeArtifactIssueTracker(),
            LicenseIssueTracker(),
        ]

        if not build_tree.is_mbedtls_3_6():
            self.issues_to_check.append(ErrorAddIssueTracker())

    def setup_logger(self, log_file, level=logging.INFO):
        """Log to log_file if provided, or to stderr if None."""
        self.logger = logging.getLogger()
        self.logger.setLevel(level)
        if log_file:
            handler = logging.FileHandler(log_file)
            self.logger.addHandler(handler)
        else:
            console = logging.StreamHandler()
            self.logger.addHandler(console)

    @staticmethod
    def collect_files():
        """Return the list of files to check.

        These are the regular files commited into Git.
        """
        bytes_output = subprocess.check_output(['git', '-C', 'framework',
                                                'ls-files', '-z'])
        bytes_framework_filepaths = bytes_output.split(b'\0')[:-1]
        bytes_framework_filepaths = ["framework/".encode() + filepath
                                     for filepath in bytes_framework_filepaths]

        bytes_output = subprocess.check_output(['git', 'ls-files', '-z'])
        bytes_filepaths = bytes_output.split(b'\0')[:-1] + \
                          bytes_framework_filepaths
        ascii_filepaths = map(lambda fp: fp.decode('ascii'), bytes_filepaths)

        # Filter out directories. Normally Git doesn't list directories
        # (it only knows about the files inside them), but there is
        # at least one case where 'git ls-files' includes a directory:
        # submodules. Just skip submodules (and any other directories).
        ascii_filepaths = [fp for fp in ascii_filepaths
                           if os.path.isfile(fp)]
        # Prepend './' to files in the top-level directory so that
        # something like `'/Makefile' in fp` matches in the top-level
        # directory as well as in subdirectories.
        return [fp if os.path.dirname(fp) else os.path.join(os.curdir, fp)
                for fp in ascii_filepaths]

    def check_files(self):
        """Check all files for all issues."""
        for issue_to_check in self.issues_to_check:
            for filepath in self.collect_files():
                if issue_to_check.should_check_file(filepath):
                    issue_to_check.check_file_for_issue(filepath)

    def output_issues(self):
        """Log the issues found and their locations.

        Return 1 if there were issues, 0 otherwise.
        """
        integrity_return_code = 0
        for issue_to_check in self.issues_to_check:
            if issue_to_check.files_with_issues:
                integrity_return_code = 1
            issue_to_check.output_file_issues(self.logger)
        return integrity_return_code


def run_main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "-l", "--log_file", type=str, help="path to optional output log",
    )
    check_args = parser.parse_args()
    integrity_check = IntegrityChecker(check_args.log_file)
    integrity_check.check_files()
    return_code = integrity_check.output_issues()
    sys.exit(return_code)


if __name__ == "__main__":
    run_main()
