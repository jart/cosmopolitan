"""Install all the required Python packages, with the minimum Python version.
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

import argparse
import os
import re
import subprocess
import sys
import tempfile
import typing

from typing import List, Optional

import framework_scripts_path # pylint: disable=unused-import
from mbedtls_framework import typing_util

def pylint_doesn_t_notice_that_certain_types_are_used_in_annotations(
        _list: List[typing.Any],
) -> None:
    pass


class Requirements:
    """Collect and massage Python requirements."""

    def __init__(self) -> None:
        self.requirements = [] #type: List[str]

    def adjust_requirement(self, req: str) -> str:
        """Adjust a requirement to the minimum specified version."""
        # allow inheritance #pylint: disable=no-self-use
        # If a requirement specifies a minimum version, impose that version.
        split_req = req.split(';', 1)
        split_req[0] = re.sub(r'>=|~=', r'==', split_req[0])
        return ';'.join(split_req)

    def add_file(self, filename: str) -> None:
        """Add requirements from the specified file.

        This method supports a subset of pip's requirement file syntax:
        * One requirement specifier per line, which is passed to
          `adjust_requirement`.
        * Comments (``#`` at the beginning of the line or after whitespace).
        * ``-r FILENAME`` to include another file.
        """
        for line in open(filename):
            line = line.strip()
            line = re.sub(r'(\A|\s+)#.*', r'', line)
            if not line:
                continue
            m = re.match(r'-r\s+', line)
            if m:
                nested_file = os.path.join(os.path.dirname(filename),
                                           line[m.end(0):])
                self.add_file(nested_file)
                continue
            self.requirements.append(self.adjust_requirement(line))

    def write(self, out: typing_util.Writable) -> None:
        """List the gathered requirements."""
        for req in self.requirements:
            out.write(req + '\n')

    def install(
            self,
            pip_general_options: Optional[List[str]] = None,
            pip_install_options: Optional[List[str]] = None,
    ) -> None:
        """Call pip to install the requirements."""
        if pip_general_options is None:
            pip_general_options = []
        if pip_install_options is None:
            pip_install_options = []
        with tempfile.TemporaryDirectory() as temp_dir:
            # This is more complicated than it needs to be for the sake
            # of Windows. Use a temporary file rather than the command line
            # to avoid quoting issues. Use a temporary directory rather
            # than NamedTemporaryFile because with a NamedTemporaryFile on
            # Windows, the subprocess can't open the file because this process
            # has an exclusive lock on it.
            req_file_name = os.path.join(temp_dir, 'requirements.txt')
            with open(req_file_name, 'w') as req_file:
                self.write(req_file)
            subprocess.check_call([sys.executable, '-m', 'pip'] +
                                  pip_general_options +
                                  ['install'] + pip_install_options +
                                  ['-r', req_file_name])

def main(default_requirement_file: str) -> None:
    """Command line entry point."""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--no-act', '-n',
                        action='store_true',
                        help="Don't act, just print what will be done")
    parser.add_argument('--pip-install-option',
                        action='append', dest='pip_install_options',
                        help="Pass this option to pip install")
    parser.add_argument('--pip-option',
                        action='append', dest='pip_general_options',
                        help="Pass this general option to pip")
    parser.add_argument('--user',
                        action='append_const', dest='pip_install_options',
                        const='--user',
                        help="Install to the Python user install directory"
                             " (short for --pip-install-option --user)")
    parser.add_argument('files', nargs='*', metavar='FILE',
                        help="Requirement files"
                             " (default: {})" \
                             .format(default_requirement_file))
    options = parser.parse_args()
    if not options.files:
        options.files = [default_requirement_file]
    reqs = Requirements()
    for filename in options.files:
        reqs.add_file(filename)
    reqs.write(sys.stdout)
    if not options.no_act:
        reqs.install(pip_general_options=options.pip_general_options,
                     pip_install_options=options.pip_install_options)
