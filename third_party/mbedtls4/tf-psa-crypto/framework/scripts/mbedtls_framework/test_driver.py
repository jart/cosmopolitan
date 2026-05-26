"""Library for building a TF-PSA-Crypto test driver from the built-in driver
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#

import argparse
import re
import shutil
import subprocess

from fnmatch import fnmatch
from pathlib import Path
from typing import Iterable, List, Match, Optional, Set

def get_parsearg_base() -> argparse.ArgumentParser:
    """ Get base arguments for scripts generating a TF-PSA-Crypto test driver """
    parser = argparse.ArgumentParser(description="""\
        Clone the built-in driver tree, rewrite header inclusions and prefix
        exposed C identifiers.
        """)

    parser.add_argument("dst_dir", metavar="DST_DIR",
                        help="Destination directory (relative to repository root)")
    parser.add_argument("--driver", default="libtestdriver1", metavar="DRIVER",
                        help="Test driver name (default: %(default)s).")
    parser.add_argument('--list-vars-for-cmake', nargs="?", \
                        const="__AUTO__", metavar="FILE",
                        help="""
        Generate a file to be included from a CMakeLists.txt and exit. The file
        defines CMake list variables with the script's inputs/outputs files. If
        FILE is omitted, the output name defaults to '<DRIVER>-list-vars.cmake'.
        """)
    return parser

class TestDriverGenerator:
    """A TF-PSA-Crypto test driver generator"""
    def __init__(self, src_dir: Path, dst_dir: Path, driver: str, \
                 exclude_files: Optional[Iterable[str]] = None) -> None:
        """
        Initialize a test driver generator.

        Args:
            src_dir (Path):
                Path to the source directory that contains the built-in driver.
                If this path is relative, it should be relative to the repository
                root so that the source paths returned by `write_list_vars_for_cmake`
                are correct.

                The source directory is expected to contain:
                - an `include` directory
                - an `src` directory

            dst_dir (Path):
                Path to the destination directory where the rewritten tree will
                be created. If the directory already exists, only the `include`
                and `src` subdirectories are modified.

            driver (str):
                Name of the driver. This is used as a prefix when rewritting
                the tree.

            exclude_files (Optional[Iterable[str]]):
                Glob patterns for the basename of the files to be excluded from
                the source directory.
        """
        self.src_dir = src_dir
        self.dst_dir = dst_dir
        self.driver = driver
        self.exclude_files = [] if exclude_files is None else list(exclude_files)

        if not (src_dir / "include").is_dir():
            raise RuntimeError(f'"include" directory in {src_dir} not found')

        if not (src_dir / "src").is_dir():
            raise RuntimeError(f'"src" directory in {src_dir} not found')

    def write_list_vars_for_cmake(self, fname: str) -> None:
        src_relpaths = self.__get_src_code_files()
        with open(self.dst_dir / fname, "w") as f:
            f.write(f"set({self.driver}_input_files\n    " + \
                     "\n    ".join(f'"{path}"' for path in src_relpaths) + "\n)\n\n")
            f.write(f"set({self.driver}_files\n    " + \
                    "\n    ".join(f'"{self.__get_dst_relpath(path.relative_to(self.src_dir))}"' \
                     for path in src_relpaths) + "\n)\n\n")
            f.write(f"set({self.driver}_src_files\n    " + \
                    "\n    ".join(f'"{self.__get_dst_relpath(path.relative_to(self.src_dir))}"' \
                    for path in src_relpaths if path.suffix == ".c") + "\n)\n")

    def get_identifiers_to_prefix(self, prefixes: Set[str]) -> Set[str]:
        """
        Get the set of identifiers that will be prefixed in the test driver code.

        This method is intended to be amended by subclasses in consuming branches.

        The default implementation returns the complete set of identifiers from
        the built-in driver whose names begin with any of the `prefixes`. These
        are the identifiers that could be renamed in the test driver before
        adaptation.

        Subclasses need to filter, transform, or otherwise adjust the set of
        identifiers that should be renamed when generating the test driver.

        Args:
             prefixes (Set[str]):
                 The set of identifier prefixes used by the built-in driver
                 for the symbols it exposes to the other parts of the crypto
                 library. All identifiers beginning with any of these
                 prefixes are candidates for renaming in the test driver to
                 avoid symbol clashes.

        Returns:
            Set[str]: The default set of identifiers to rename.
        """
        identifiers = set()
        for file in self.__get_src_code_files():
            identifiers.update(self.get_c_identifiers(file))

        identifiers_with_prefixes = set()
        for identifier in identifiers:
            if any(identifier.startswith(prefix) for prefix in prefixes):
                identifiers_with_prefixes.add(identifier)
        return identifiers_with_prefixes

    def create_test_driver_tree(self, prefixes: Set[str]) -> None:
        """
        Create a test driver tree from `self.src_dir` into `self.dst_dir`.

        Only the `include/` and `src/` subdirectories of the source tree are
        used, and their internal directory structure is preserved.

        Only "*.h" and "*.c" files are copied. Files whose basenames match any
        of the glob patterns in `self.exclude_files` are excluded.

        The basename of all files is prefixed with `{self.driver}-`. The
        header inclusions referencing the renamed headers are rewritten
        accordingly.

        Symbol identifiers exposed by the built-in driver are renamed by
        prefixing them with `{self.driver}_` to avoid collisions when linking the
        built-in driver and the test driver together in the crypto library.

        Args:
             prefixes (Set[str]):
                 The set of identifier prefixes used by the built-in driver
                 for the symbols it exposes to the other parts of the crypto
                 library. All identifiers beginning with any of these
                 prefixes are candidates for renaming in the test driver to
                 avoid symbol clashes.
        """
        if (self.dst_dir / "include").exists():
            shutil.rmtree(self.dst_dir / "include")

        if (self.dst_dir / "src").exists():
            shutil.rmtree(self.dst_dir / "src")

        headers = {
            f.name \
            for f in self.__get_src_code_files() if f.suffix == ".h"
        }
        identifiers_to_prefix = self.get_identifiers_to_prefix(prefixes)

        # Create the test driver tree
        for file in self.__get_src_code_files():
            dst = self.dst_dir / \
                  self.__get_dst_relpath(file.relative_to(self.src_dir))
            dst.parent.mkdir(parents=True, exist_ok=True)
            self.__write_test_driver_file(file, dst, headers,\
                                          identifiers_to_prefix)

    @staticmethod
    def __get_code_files(root: Path) -> List[Path]:
        """
        Return all "*.c" and "*.h" files found recursively under the
        `include` and `src` subdirectories of `root`.
        """
        return sorted(path
                      for directory in ('include', 'src')
                      for path in (root / directory).rglob('*.[hc]'))

    def __get_src_code_files(self) -> List[Path]:
        """
        Return all "*.c" and "*.h" files found recursively under the
        `include` and `src` subdirectories of the source directory `self.src_dir`
        excluding the files whose basename match any of the patterns in
        `self.exclude_files`.
        """
        out = []
        for file in self.__get_code_files(self.src_dir):
            if not any(fnmatch(file.name, pattern) for pattern in self.exclude_files):
                out.append(file)
        return out

    def __get_dst_relpath(self, src_relpath: Path) -> Path:
        """
        Return the path relative to `dst_dir` of the file that corresponds to the
        file with relative path `src_relpath` in the source tree.

        Same as `src_relpath` but the basename prefixed with `self.driver`
        """
        assert not src_relpath.is_absolute(), "src_relpath must be relative"

        return src_relpath.parent / (self.driver + '-' + src_relpath.name)

    @staticmethod
    def get_c_identifiers(file: Path) -> Set[str]:
        """
        Extract the C identifiers present in `file` using `ctags -x`

        The following C symbol kinds are included (with their `--c-kinds`
        flags in parentheses):

          - macro definitions (d)
          - enum values (e)
          - functions (f)
          - enum tags (g)
          - function prototypes (p)
          - struct tags (s)
          - typedefs (t)
          - union tags (u)
          - global variables (v)

        Compatibility
        -------------
        The command used here has been validated with the following `ctags`
        implementations:
          - Exuberant Ctags 5.8
          - Exuberant Ctags 5.9~svn20110310 (default on Ubuntu 16.04–24.04)
          - Universal Ctags 5.9.0 (Ubuntu 24.04)
          - Universal Ctags 6.2.0 (Ubuntu 26.04)

        All of these versions support the options `-x`, `--language-force=C`,
        and ``--c-kinds=defgpstuv`` sufficiently for the use case here.

        Returns:
            Set[str]: The set of identifiers found in `file`.
        """
        output = subprocess.check_output(
            ["ctags", "-x", "--language-force=C", "--c-kinds=defgpstuv", str(file)],
            stderr=subprocess.STDOUT,
            universal_newlines=True,
        )
        identifiers = set()
        for line in output.splitlines():
            identifiers.add(line.split()[0])

        return identifiers

    def __write_test_driver_file(self, src: Path, dst: Path,
                                 headers: Set[str],
                                 identifiers_to_prefix: Set[str]) -> None:
        """
        Write a test driver file to `dst` based on the contents of `src` with
        two transformations: rewriting of `#include` directives and identifier
        renaming.

        1. Rewrite header inclusions
           Any `#include` directive whose header basename matches an entry of
           `headers` is rewritten so that the basename is prefixed with
           `{self.driver}-`. Directory components (if any) are preserved.

           Example:
               #include "mbedtls/private/aes.h"
               becomes
               #include "mbedtls/private/libtestdriver1-aes.h"

           LIMITATION:
               The current implementation does not correctly handle the case
               where a built-in header and a non–built-in header share the same
               basename. In principle, only inclusions of built-in headers
               should be rewritten, while inclusions of non–built-in headers
               should be left unchanged. However, the current logic only matches
               on the basename, so both are rewritten.

               For example, if both `include/psa/foo.h` (non–built-in) and
               `drivers/builtin/include/mbedtls/foo.h` (built-in) exist, then
               in the test driver:

               - `#include <psa/foo.h>` should not be rewritten
               - `#include <mbedtls/foo.h>` should be rewritten to
                 `#include <mbedtls/libtestdriver1-foo.h>`

               With the current basename-only matching, both inclusions are
               rewritten, which is incorrect. No practical implications
               currently, such same header basename case does not occur in the
               code base.

        2. Rename selected identifiers
           Each identifier in `identifiers_to_prefix` is prefixed with
           `self.driver`. Case is preserved: if the identifier is all-uppercase,
           then the uppercase form of `driver` is used, the lowercase form
           otherwise.

           Examples:
               `MBEDTLS_AES_C` becomes `LIBTESTDRIVER1_MBEDTLS_AES_C`
               `mbedtls_sha256_init` becomes `libtestdriver1_mbedtls_sha256_init`

        Args:
            src (Path):
                The source file to read.

            dst (Path):
                The destination file where the rewritten version is written.

            headers (Set[str]):
                Basenames of headers whose includes should be rewritten.

            identifiers_to_prefix (Set[str]):
                Identifiers that must be renamed by prefixing with `self.driver`
                (using uppercase or lowercase depending on the identifier's casing).
        """
        text = src.read_text(encoding="utf-8")

        include_line_re = re.compile(
            fr'^(\s*#\s*include\s*[<"])([^>"]+)([>"])',
            re.MULTILINE
        )
        def repl_header_inclusion(m: Match) -> str:
            parts = m.group(2).split("/")
            if parts[-1] in headers:
                path = "/".join(parts[:-1] + [self.driver + "-" + parts[-1]])
                return f'{m.group(1)}{path}{m.group(3)}'
            return m.group(0)
        intermediate_text = include_line_re.sub(repl_header_inclusion, text)

        c_identifier_re = re.compile(r"\b[A-Za-z_][A-Za-z0-9_]*\b")
        prefix_uppercased = self.driver.upper()
        prefix_lowercased = self.driver.lower()

        def repl(m: Match) -> str:
            identifier = m.group(0)
            if identifier in identifiers_to_prefix:
                if identifier[0].isupper():
                    return f"{prefix_uppercased}_{identifier}"
                else:
                    return f"{prefix_lowercased}_{identifier}"
            return identifier

        new_text = c_identifier_re.sub(repl, intermediate_text)
        dst.write_text(new_text, encoding="utf-8")
