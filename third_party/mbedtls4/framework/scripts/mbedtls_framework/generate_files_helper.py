"""Utilities for intermediate files that are generated, but platform-independent
and configuration-independent.
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

import argparse
import os
import subprocess
import sys
from typing import Dict, Iterable, List, Sequence, Set


class Generator:
    """An abstract base class for generators of intermediate files."""

    def generator_name(self) -> str:
        """A name for this generator.

        Generator names must be unique and should not be identical to
        the name of any target.
        """
        raise NotImplementedError

    def target_files(self) -> List[str]:
        """The list of files targeted by this generator.

        File names are relative to the project root.
        """
        raise NotImplementedError

    def outdated_files(self) -> Iterable[str]:
        """Return the list of targets that are out of date.

        This is empty after running update().
        Missing targets are considered out of date.
        """
        raise NotImplementedError

    def update(self, always: bool) -> None:
        """Update the target(s) of this generator.

        If always is false, avoid changing the output file if it already has
        the desired content. If always is true, make sure to update the
        time stamp on the output file even if it already has the desired content.
        """
        raise NotImplementedError


class TestDataGenerator(Generator):
    """A test data generator script.

    Even though the test data generator scripts are written in Python, we
    run them as a separate process, because their output depends on the
    program name (they write sys.argv[0] in a comment in the .data file).
    """

    def __init__(self, script: str) -> None:
        """Run the specified test generator to generate files.

        Assume that the script is written in Python and has the command line
        interface of test_data_generation.py.
        """
        self.script = script

    def generator_name(self) -> str:
        return os.path.basename(self.script)

    def target_files(self) -> List[str]:
        output = subprocess.check_output([sys.executable, self.script, '--list'],
                                         encoding='utf-8')
        return output.splitlines()

    def outdated_files(self) -> List[str]:
        output = subprocess.check_output([sys.executable, self.script, '--list-outdated'],
                                         encoding='utf-8')
        return output.splitlines()

    def update(self, _always) -> None:
        subprocess.check_call([sys.executable, self.script])


def assemble(available: Iterable[Generator]) -> Dict[str, Generator]:
    """Assemble the generators into a dictionary with both names and targets as keys."""
    by_ident = {} #type: Dict[str, Generator]
    for generator in available:
        ident = generator.generator_name()
        if ident in by_ident:
            raise Exception(f'Generator conflict: name "{ident}" of {generator} '
                            f'already recorded for {by_ident[ident]}')
        by_ident[ident] = generator
        for ident in generator.target_files():
            if ident in by_ident:
                raise Exception(f'Generator conflict: target "{ident}" of {generator} '
                                f'already recorded for {by_ident[ident]}')
            by_ident[ident] = generator
    return by_ident

def list_names(available: Iterable[Generator]) -> List[str]:
    """Return the list of generator names."""
    return sorted(generator.generator_name() for generator in available)

def list_targets(available: Iterable[Generator]) -> List[str]:
    """Return the list of generator targets."""
    return sorted(target
                  for generator in available
                  for target in generator.target_files())

def select(available: Dict[str, Generator],
           wanted: Iterable[str]) -> List[Generator]:
    """Select generators by name or target."""
    wanted_names = set() #type: Set[str]
    for ident in wanted:
        if ident not in available:
            raise Exception(f'No generator found for {ident}')
        wanted_names.add(ident)
    return [available[name] for name in sorted(wanted_names)]

def main(generators: Sequence[Generator],
         description: str) -> None:
    #pylint: disable=too-many-branches
    """Command line entry point.
    """
    parser = argparse.ArgumentParser(description=description)
    parser.add_argument('--always-update', '-U',
                        action='store_true',
                        help=('Update target files unconditionally '
                              '(overrides --update)'))
    parser.add_argument('--list',
                        action='store_true',
                        help='List generator names and targets and exit')
    parser.add_argument('--list-names',
                        action='store_true',
                        help='List generator names and exit')
    parser.add_argument('--list-targets',
                        action='store_true',
                        help='List generator targets and exit')
    parser.add_argument('--update', '-u',
                        action='store_true',
                        help='Update target files if needed')
    parser.add_argument('--verbose', '-v',
                        action='store_true',
                        help='Be more verbose')
    parser.add_argument('idents', nargs='*', metavar='NAME|TARGET',
                        help='List of generator names or targets (all targets if empty)')
    args = parser.parse_args()

    if args.list:
        args.list_names = True
        args.list_targets = True
    if args.list_names:
        for name in list_names(generators):
            print(name)
    if args.list_targets:
        for target in list_targets(generators):
            print(target)
    if args.list_names or args.list_targets:
        return

    if args.idents:
        available = assemble(generators)
        wanted = select(available, args.idents) #type: Sequence[Generator]
    else:
        wanted = generators
    if args.update or args.always_update:
        for generator in wanted:
            if args.verbose:
                sys.stderr.write(f'Running generator {generator.generator_name()}...\n')
            generator.update(args.always_update)
    else:
        outdated = [] #type: List[str]
        for generator in wanted:
            if args.verbose:
                sys.stderr.write(f'Checking targets of generator {generator.generator_name()}...\n')
            outdated += generator.outdated_files()
        if outdated:
            sys.stderr.write(f'Some targets are missing or out of date.\n')
            for target in outdated:
                print(target)
            sys.stderr.write(f'Run {sys.argv[0]} -u and commit the result.')
            sys.exit(1)
