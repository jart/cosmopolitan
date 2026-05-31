"""Historical information about the library configuration.

Note: this module is deprecated. Use config_macros.py instead.
"""

## Copyright The Mbed TLS Contributors
## SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

import glob
import os
import re
from typing import Dict, FrozenSet

from . import build_tree

class ConfigHistory:
    """Historical information about the library configuration.
    """

    @staticmethod
    def load_options(filename: str) -> FrozenSet[str]:
        """Load the options from the given file.

        The file must contain one option name per line, with no whitespace.
        """
        with open(filename) as inp:
            return frozenset(inp.read().splitlines())

    def load_history_files(self,
                           variety: str,
                           exclude_dict: Dict[str, Dict[str, FrozenSet[str]]]
                           ) -> Dict[str, Dict[str, FrozenSet[str]]]:
        """Load all files containing macro lists of the given variety"""
        data = {} #type: Dict[str, Dict[str, FrozenSet[str]]]
        glob_pattern = os.path.join(self._history_dir,
                                    f'config-{variety}-*-*.txt')
        for filename in sorted(glob.glob(glob_pattern)):
            basename = os.path.splitext(os.path.basename(filename))[0]
            m = re.match(r'config-\w+-(.*?)-(.*)', basename)
            assert m is not None
            (product, version) = m.groups()
            options = self.load_options(filename)
            exclude = exclude_dict.get(product, {}).get(version, frozenset())
            data.setdefault(product, {})[version] = options - exclude
        return data

    def __init__(self) -> None:
        """Load all files containing historical option lists."""
        self._history_dir = os.path.join(build_tree.framework_root(), 'history')
        self._options = self.load_history_files('options', {})
        self._internal = self.load_history_files('adjust', self._options)

    def options(self, product: str, version: str) -> FrozenSet[str]:
        """The set of options in the given product version."""
        return self._options[product][version]

    def internal(self, product: str, version: str) -> FrozenSet[str]:
        """The set of internal option-like macros in the given product version."""
        return self._internal[product][version]
