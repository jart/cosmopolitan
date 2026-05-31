"""Information about configuration macros and derived macros."""

## Copyright The Mbed TLS Contributors
## SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

import glob
import os
import re
from typing import FrozenSet, Iterable, Iterator, List

from . import build_tree
from . import generate_files_helper


class ConfigMacros:
    """Information about configuration macros and derived macros."""

    def __init__(self, public: FrozenSet[str], adjusted: FrozenSet[str]) -> None:
        self._public = public
        self._internal = adjusted - public

    def options(self) -> FrozenSet[str]:
        """The set of configuration options in this product."""
        return self._public

    def internal(self) -> FrozenSet[str]:
        """The set of internal option-like macros in this product."""
        return self._internal

    @staticmethod
    def _load_file(filename: str) -> FrozenSet[str]:
        """Load macro names from the given file."""
        with open(filename, encoding='ascii') as input_:
            return frozenset(line.strip()
                             for line in input_)


class Current(ConfigMacros, generate_files_helper.Generator):
    """Information about config-like macros parsed from the source code."""

    _SHADOW_FILE = 'scripts/data_files/config-options-current.txt'

    _PUBLIC_CONFIG_HEADERS = [
        'include/mbedtls/mbedtls_config.h',
        'include/psa/crypto_config.h',
    ]

    _ADJUST_CONFIG_HEADERS = [
        'include/**/*adjust*.h',
        'drivers/*/include/**/*adjust*.h',
    ]

    _DEFINE_RE = re.compile(r'[/ ]*# *define  *([A-Z_a-z][0-9A-Z_a-z]*)')

    def _list_files(self, patterns: Iterable[str]) -> Iterator[str]:
        """Yield files matching the given glob patterns."""
        for pattern in patterns:
            yield from glob.glob(os.path.join(self._root, self._submodule,
                                              pattern),
                                 recursive=True)

    def _search_file(self, filename: str) -> Iterator[str]:
        """Yield macros defined in the given file."""
        with open(filename, encoding='utf-8') as input_:
            for line in input_:
                m = self._DEFINE_RE.match(line)
                if m:
                    yield m.group(1)

    def _search_files(self, patterns: Iterable[str]) -> FrozenSet[str]:
        """Yield macros defined in files matching the given glob patterns."""
        return frozenset(element
                         for filename in self._list_files(patterns)
                         for element in self._search_file(filename))

    def shadow_file_path(self) -> str:
        """The path to the option list shadow file."""
        return os.path.join(self._root, self._submodule, self._SHADOW_FILE)

    def __init__(self, submodule: str = '',
                 shadow_missing_ok: bool = False) -> None:
        """Look for macros defined in the given submodule's source tree.

        If submodule is omitted or empty, look in the root module.

        If shadow_missing_ok is true, treat a missing shadow file as
        if it was empty. This is intended for use only when regenerating
        the shadow file.
        """
        self._root = build_tree.guess_project_root()
        self._submodule = submodule
        shadow_file = self.shadow_file_path()
        try:
            public = self._load_file(shadow_file)
        except FileNotFoundError:
            if not shadow_missing_ok:
                raise
            public = frozenset()
        adjusted = self._search_files(self._ADJUST_CONFIG_HEADERS)
        super().__init__(public, adjusted)

    def live_config_options(self) -> FrozenSet[str]:
        """Return config options from the config file (as opposed to the shadow file)."""
        return self._search_files(self._PUBLIC_CONFIG_HEADERS)

    def is_shadow_file_up_to_date(self) -> bool:
        """Whether the config options shadow file is up to date."""
        live = self.live_config_options()
        return live == self._public

    def compare_shadow_file(self) -> str:
        """Compare the option list shadow file with the live config file.

        Return a string containing the names that are only found in one of
        them, in a diff-like format: a line prefixed by ``+`` if the name
        is missing from the shadow file, or by ``-`` if the name is only
        in the shadow file.
        """
        live = self.live_config_options()
        diff = []
        for x in sorted(live | self._public):
            if x not in live:
                diff.append('+' + x + '\n')
            elif x not in self._public:
                diff.append('-' + x + '\n')
        return ''.join(diff)

    def update_shadow_file(self, always_update: bool) -> None:
        """Update the shadow file from the live config file.

        If always_update is false and the shadow file already has the desired
        content, don't touch it.
        """
        if not always_update and self.is_shadow_file_up_to_date():
            return
        with open(self.shadow_file_path(), 'w') as out:
            for name in sorted(self.live_config_options()):
                out.write(name + '\n')

    # Implement the generate_files_helper.Generator interface
    def generator_name(self) -> str:
        """Name as a generate_files_helper.Generator."""
        return 'options'

    def target_files(self) -> List[str]:
        """List the (single) generated file name."""
        return [os.path.join(self._submodule, self._SHADOW_FILE)]

    def outdated_files(self) -> List[str]:
        """List the (single) generated file name if it is out of date."""
        if self.is_shadow_file_up_to_date():
            return []
        else:
            return self.target_files()

    def update(self, always: bool) -> None:
        """Update the shadow file from the live config file."""
        self.update_shadow_file(always)


class History(ConfigMacros):
    """Information about config-like macros in a previous version.

    Load files created by ``framework/scripts/save_config_history.sh``.
    """

    def _load_history_file(self, basename: str) -> FrozenSet[str]:
        """Load macro names from the given file in the history directory."""
        filename = os.path.join(self._history_dir, basename)
        return self._load_file(filename)

    def __init__(self, project: str, version: str) -> None:
        """Read information about the given project at the given version.

        The information must be present in history files in the framework.
        """
        self._history_dir = os.path.join(build_tree.framework_root(), 'history')
        public = self._load_history_file(f'config-options-{project}-{version}.txt')
        adjusted = self._load_history_file(f'config-adjust-{project}-{version}.txt')
        super().__init__(public, adjusted)
