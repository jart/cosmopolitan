#!/usr/bin/env python3

"""
This script is for comparing the size of the library files from two
different Git revisions within an Mbed TLS repository.
The results of the comparison is formatted as csv and stored at a
configurable location.
Note: must be run from Mbed TLS root.
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

import argparse
import logging
import os
import re
import shutil
import subprocess
import sys
import typing
from enum import Enum

import framework_scripts_path # pylint: disable=unused-import
from mbedtls_framework import build_tree
from mbedtls_framework import logging_util
from mbedtls_framework import typing_util

class SupportedArch(Enum):
    """Supported architecture for code size measurement."""
    AARCH64 = 'aarch64'
    AARCH32 = 'aarch32'
    ARMV8_M = 'armv8-m'
    X86_64 = 'x86_64'
    X86 = 'x86'


class SupportedConfig(Enum):
    """Supported configuration for code size measurement."""
    DEFAULT = 'default'
    TFM_MEDIUM = 'tfm-medium'


# Static library
MBEDTLS_STATIC_LIB = {
    'CRYPTO': 'library/libmbedcrypto.a',
    'X509': 'library/libmbedx509.a',
    'TLS': 'library/libmbedtls.a',
}

class CodeSizeDistinctInfo: # pylint: disable=too-few-public-methods
    """Data structure to store possibly distinct information for code size
    comparison."""
    def __init__( #pylint: disable=too-many-arguments
            self,
            version: str,
            git_rev: str,
            arch: str,
            config: str,
            compiler: str,
            opt_level: str,
    ) -> None:
        """
        :param: version: which version to compare with for code size.
        :param: git_rev: Git revision to calculate code size.
        :param: arch: architecture to measure code size on.
        :param: config: Configuration type to calculate code size.
                        (See SupportedConfig)
        :param: compiler: compiler used to build library/*.o.
        :param: opt_level: Options that control optimization. (E.g. -Os)
        """
        self.version = version
        self.git_rev = git_rev
        self.arch = arch
        self.config = config
        self.compiler = compiler
        self.opt_level = opt_level
        # Note: Variables below are not initialized by class instantiation.
        self.pre_make_cmd = [] #type: typing.List[str]
        self.make_cmd = ''

    def get_info_indication(self):
        """Return a unique string to indicate Code Size Distinct Information."""
        return '{git_rev}-{arch}-{config}-{compiler}'.format(**self.__dict__)


class CodeSizeCommonInfo: # pylint: disable=too-few-public-methods
    """Data structure to store common information for code size comparison."""
    def __init__(
            self,
            host_arch: str,
            measure_cmd: str,
    ) -> None:
        """
        :param host_arch: host architecture.
        :param measure_cmd: command to measure code size for library/*.o.
        """
        self.host_arch = host_arch
        self.measure_cmd = measure_cmd

    def get_info_indication(self):
        """Return a unique string to indicate Code Size Common Information."""
        return '{measure_tool}'\
               .format(measure_tool=self.measure_cmd.strip().split(' ')[0])

class CodeSizeResultInfo: # pylint: disable=too-few-public-methods
    """Data structure to store result options for code size comparison."""
    def __init__( #pylint: disable=too-many-arguments
            self,
            record_dir: str,
            comp_dir: str,
            with_markdown=False,
            stdout=False,
            show_all=False,
    ) -> None:
        """
        :param record_dir: directory to store code size record.
        :param comp_dir: directory to store results of code size comparision.
        :param with_markdown: write comparision result into a markdown table.
                              (Default: False)
        :param stdout: direct comparison result into sys.stdout.
                       (Default False)
        :param show_all: show all objects in comparison result. (Default False)
        """
        self.record_dir = record_dir
        self.comp_dir = comp_dir
        self.with_markdown = with_markdown
        self.stdout = stdout
        self.show_all = show_all


DETECT_ARCH_CMD = "cc -dM -E - < /dev/null"
def detect_arch() -> str:
    """Auto-detect host architecture."""
    cc_output = subprocess.check_output(DETECT_ARCH_CMD, shell=True).decode()
    if '__aarch64__' in cc_output:
        return SupportedArch.AARCH64.value
    if '__arm__' in cc_output:
        return SupportedArch.AARCH32.value
    if '__x86_64__' in cc_output:
        return SupportedArch.X86_64.value
    if '__i386__' in cc_output:
        return SupportedArch.X86.value
    else:
        print("Unknown host architecture, cannot auto-detect arch.")
        sys.exit(1)

TFM_MEDIUM_CONFIG_H = 'configs/ext/tfm_mbedcrypto_config_profile_medium.h'
TFM_MEDIUM_CRYPTO_CONFIG_H = 'tf-psa-crypto/configs/ext/crypto_config_profile_medium.h'

CONFIG_H = 'include/mbedtls/mbedtls_config.h'
CRYPTO_CONFIG_H = 'tf-psa-crypto/include/psa/crypto_config.h'
BACKUP_SUFFIX = '.code_size.bak'

class CodeSizeBuildInfo: # pylint: disable=too-few-public-methods
    """Gather information used to measure code size.

    It collects information about architecture, configuration in order to
    infer build command for code size measurement.
    """

    SupportedArchConfig = [
        '-a ' + SupportedArch.AARCH64.value + ' -c ' + SupportedConfig.DEFAULT.value,
        '-a ' + SupportedArch.AARCH32.value + ' -c ' + SupportedConfig.DEFAULT.value,
        '-a ' + SupportedArch.X86_64.value  + ' -c ' + SupportedConfig.DEFAULT.value,
        '-a ' + SupportedArch.X86.value     + ' -c ' + SupportedConfig.DEFAULT.value,
        '-a ' + SupportedArch.ARMV8_M.value + ' -c ' + SupportedConfig.TFM_MEDIUM.value,
    ]

    def __init__(
            self,
            size_dist_info: CodeSizeDistinctInfo,
            host_arch: str,
            logger: logging.Logger,
    ) -> None:
        """
        :param size_dist_info:
            CodeSizeDistinctInfo containing info for code size measurement.
                - size_dist_info.arch: architecture to measure code size on.
                - size_dist_info.config: configuration type to measure
                                         code size with.
                - size_dist_info.compiler: compiler used to build library/*.o.
                - size_dist_info.opt_level: Options that control optimization.
                                            (E.g. -Os)
        :param host_arch: host architecture.
        :param logger: logging module
        """
        self.arch = size_dist_info.arch
        self.config = size_dist_info.config
        self.compiler = size_dist_info.compiler
        self.opt_level = size_dist_info.opt_level

        self.make_cmd = ['make', '-f', './scripts/legacy.make', '-j', 'lib']

        self.host_arch = host_arch
        self.logger = logger

    def check_correctness(self) -> bool:
        """Check whether we are using proper / supported combination
        of information to build library/*.o."""

        # default config
        if self.config == SupportedConfig.DEFAULT.value and \
            self.arch == self.host_arch:
            return True
        # TF-M
        elif self.arch == SupportedArch.ARMV8_M.value and \
             self.config == SupportedConfig.TFM_MEDIUM.value:
            return True

        return False

    def infer_pre_make_command(self) -> typing.List[str]:
        """Infer command to set up proper configuration before running make."""
        pre_make_cmd = [] #type: typing.List[str]
        if self.config == SupportedConfig.TFM_MEDIUM.value:
            pre_make_cmd.append('cp {src} {dest}'
                                .format(src=TFM_MEDIUM_CONFIG_H, dest=CONFIG_H))
            pre_make_cmd.append('cp {src} {dest}'
                                .format(src=TFM_MEDIUM_CRYPTO_CONFIG_H,
                                        dest=CRYPTO_CONFIG_H))

        return pre_make_cmd

    def infer_make_cflags(self) -> str:
        """Infer CFLAGS by instance attributes in CodeSizeDistinctInfo."""
        cflags = [] #type: typing.List[str]

        # set optimization level
        cflags.append(self.opt_level)
        # set compiler by config
        if self.config == SupportedConfig.TFM_MEDIUM.value:
            self.compiler = 'armclang'
            cflags.append('-mcpu=cortex-m33')
        # set target
        if self.compiler == 'armclang':
            cflags.append('--target=arm-arm-none-eabi')

        return ' '.join(cflags)

    def infer_make_command(self) -> str:
        """Infer make command by CFLAGS and CC."""

        if self.check_correctness():
            # set CFLAGS=
            self.make_cmd.append('CFLAGS=\'{}\''.format(self.infer_make_cflags()))
            # set CC=
            self.make_cmd.append('CC={}'.format(self.compiler))
            return ' '.join(self.make_cmd)
        else:
            self.logger.error("Unsupported combination of architecture: {} " \
                              "and configuration: {}.\n"
                              .format(self.arch,
                                      self.config))
            self.logger.error("Please use supported combination of " \
                             "architecture and configuration:")
            for comb in CodeSizeBuildInfo.SupportedArchConfig:
                self.logger.error(comb)
            self.logger.error("")
            self.logger.error("For your system, please use:")
            for comb in CodeSizeBuildInfo.SupportedArchConfig:
                if "default" in comb and self.host_arch not in comb:
                    continue
                self.logger.error(comb)
            sys.exit(1)


class CodeSizeCalculator:
    """ A calculator to calculate code size of library/*.o based on
    Git revision and code size measurement tool.
    """

    def __init__( #pylint: disable=too-many-arguments
            self,
            git_rev: str,
            pre_make_cmd: typing.List[str],
            make_cmd: str,
            measure_cmd: str,
            logger: logging.Logger,
    ) -> None:
        """
        :param git_rev: Git revision. (E.g: commit)
        :param pre_make_cmd: command to set up proper config before running make.
        :param make_cmd: command to build library/*.o.
        :param measure_cmd: command to measure code size for library/*.o.
        :param logger: logging module
        """
        self.repo_path = "."
        self.git_command = "git"
        self.make_clean = 'make -f ./scripts/legacy.make clean'

        self.git_rev = git_rev
        self.pre_make_cmd = pre_make_cmd
        self.make_cmd = make_cmd
        self.measure_cmd = measure_cmd
        self.logger = logger

    @staticmethod
    def validate_git_revision(git_rev: str) -> str:
        result = subprocess.check_output(["git", "rev-parse", "--verify",
                                          git_rev + "^{commit}"],
                                         shell=False, universal_newlines=True)
        return result[:7]

    def _create_git_worktree(self) -> str:
        """Create a separate worktree for Git revision.
        If Git revision is current, use current worktree instead."""

        if self.git_rev == 'current':
            self.logger.debug("Using current work directory.")
            git_worktree_path = self.repo_path
        else:
            self.logger.debug("Creating git worktree for {}."
                              .format(self.git_rev))
            git_worktree_path = os.path.join(self.repo_path,
                                             "temp-" + self.git_rev)
            subprocess.check_output(
                [self.git_command, "worktree", "add", "--detach",
                 git_worktree_path, self.git_rev], cwd=self.repo_path,
                stderr=subprocess.STDOUT
            )
            subprocess.check_output(
                [self.git_command, "submodule", "update", "--init", "--recursive"],
                cwd=git_worktree_path, stderr=subprocess.STDOUT
            )

        return git_worktree_path

    @staticmethod
    def backup_config_files(restore: bool) -> None:
        """Backup / Restore config files."""
        if restore:
            shutil.move(CONFIG_H + BACKUP_SUFFIX, CONFIG_H)
            shutil.move(CRYPTO_CONFIG_H + BACKUP_SUFFIX, CRYPTO_CONFIG_H)
        else:
            shutil.copy(CONFIG_H, CONFIG_H + BACKUP_SUFFIX)
            shutil.copy(CRYPTO_CONFIG_H, CRYPTO_CONFIG_H + BACKUP_SUFFIX)

    def _build_libraries(self, git_worktree_path: str) -> None:
        """Build library/*.o in the specified worktree."""

        self.logger.debug("Building library/*.o for {}."
                          .format(self.git_rev))
        my_environment = os.environ.copy()
        try:
            if self.git_rev == 'current':
                self.backup_config_files(restore=False)
            for pre_cmd in self.pre_make_cmd:
                subprocess.check_output(
                    pre_cmd, env=my_environment, shell=True,
                    cwd=git_worktree_path, stderr=subprocess.STDOUT,
                    universal_newlines=True
                )
            subprocess.check_output(
                self.make_clean, env=my_environment, shell=True,
                cwd=git_worktree_path, stderr=subprocess.STDOUT,
                universal_newlines=True
            )
            subprocess.check_output(
                self.make_cmd, env=my_environment, shell=True,
                cwd=git_worktree_path, stderr=subprocess.STDOUT,
                universal_newlines=True
            )
            if self.git_rev == 'current':
                self.backup_config_files(restore=True)
        except subprocess.CalledProcessError as e:
            self._handle_called_process_error(e, git_worktree_path)

    def _gen_raw_code_size(self, git_worktree_path: str) -> typing.Dict[str, str]:
        """Measure code size by a tool and return in UTF-8 encoding."""

        self.logger.debug("Measuring code size for {} by `{}`."
                          .format(self.git_rev,
                                  self.measure_cmd.strip().split(' ')[0]))

        res = {}
        for mod, st_lib in MBEDTLS_STATIC_LIB.items():
            try:
                result = subprocess.check_output(
                    [self.measure_cmd + ' ' + st_lib], cwd=git_worktree_path,
                    shell=True, universal_newlines=True
                )
                res[mod] = result
            except subprocess.CalledProcessError as e:
                self._handle_called_process_error(e, git_worktree_path)

        return res

    def _remove_worktree(self, git_worktree_path: str) -> None:
        """Remove temporary worktree."""
        if git_worktree_path != self.repo_path:
            self.logger.debug("Removing temporary worktree {}."
                              .format(git_worktree_path))
            subprocess.check_output(
                [self.git_command, "worktree", "remove", "--force",
                 git_worktree_path], cwd=self.repo_path,
                stderr=subprocess.STDOUT
            )

    def _handle_called_process_error(self, e: subprocess.CalledProcessError,
                                     git_worktree_path: str) -> None:
        """Handle a CalledProcessError and quit the program gracefully.
        Remove any extra worktrees so that the script may be called again."""

        # Tell the user what went wrong
        self.logger.error(e, exc_info=True)
        self.logger.error("Process output:\n {}".format(e.output))

        # Quit gracefully by removing the existing worktree
        self._remove_worktree(git_worktree_path)
        sys.exit(-1)

    def cal_libraries_code_size(self) -> typing.Dict[str, str]:
        """Do a complete round to calculate code size of library/*.o
        by measurement tool.

        :return A dictionary of measured code size
            - typing.Dict[mod: str]
        """

        git_worktree_path = self._create_git_worktree()
        try:
            self._build_libraries(git_worktree_path)
            res = self._gen_raw_code_size(git_worktree_path)
        finally:
            self._remove_worktree(git_worktree_path)

        return res


class CodeSizeGenerator:
    """ A generator based on size measurement tool for library/*.o.

    This is an abstract class. To use it, derive a class that implements
    write_record and write_comparison methods, then call both of them with
    proper arguments.
    """
    def __init__(self, logger: logging.Logger) -> None:
        """
        :param logger: logging module
        """
        self.logger = logger

    def write_record(
            self,
            git_rev: str,
            code_size_text: typing.Dict[str, str],
            output: typing_util.Writable
    ) -> None:
        """Write size record into a file.

        :param git_rev: Git revision. (E.g: commit)
        :param code_size_text:
            string output (utf-8) from measurement tool of code size.
                - typing.Dict[mod: str]
        :param output: output stream which the code size record is written to.
                       (Note: Normally write code size record into File)
        """
        raise NotImplementedError

    def write_comparison( #pylint: disable=too-many-arguments
            self,
            old_rev: str,
            new_rev: str,
            output: typing_util.Writable,
            with_markdown=False,
            show_all=False
    ) -> None:
        """Write a comparision result into a stream between two Git revisions.

        :param old_rev: old Git revision to compared with.
        :param new_rev: new Git revision to compared with.
        :param output: output stream which the code size record is written to.
                       (File / sys.stdout)
        :param with_markdown:  write comparision result in a markdown table.
                               (Default: False)
        :param show_all: show all objects in comparison result. (Default False)
        """
        raise NotImplementedError


class CodeSizeGeneratorWithSize(CodeSizeGenerator):
    """Code Size Base Class for size record saving and writing."""

    class SizeEntry: # pylint: disable=too-few-public-methods
        """Data Structure to only store information of code size."""
        def __init__(self, text: int, data: int, bss: int, dec: int):
            self.text = text
            self.data = data
            self.bss = bss
            self.total = dec # total <=> dec

    def __init__(self, logger: logging.Logger) -> None:
        """ Variable code_size is used to store size info for any Git revisions.
        :param code_size:
            Data Format as following:
            code_size = {
                git_rev: {
                    module: {
                        file_name: SizeEntry,
                        ...
                    },
                    ...
                },
                ...
            }
        """
        super().__init__(logger)
        self.code_size = {} #type: typing.Dict[str, typing.Dict]
        self.mod_total_suffix = '-' + 'TOTALS'

    def _set_size_record(self, git_rev: str, mod: str, size_text: str) -> None:
        """Store size information for target Git revision and high-level module.

        size_text Format: text data bss dec hex filename
        """
        size_record = {}
        for line in size_text.splitlines()[1:]:
            data = line.split()
            if re.match(r'\s*\(TOTALS\)', data[5]):
                data[5] = mod + self.mod_total_suffix
            # file_name: SizeEntry(text, data, bss, dec)
            size_record[data[5]] = CodeSizeGeneratorWithSize.SizeEntry(
                int(data[0]), int(data[1]), int(data[2]), int(data[3]))
        self.code_size.setdefault(git_rev, {}).update({mod: size_record})

    def read_size_record(self, git_rev: str, fname: str) -> None:
        """Read size information from csv file and write it into code_size.

        fname Format: filename text data bss dec
        """
        mod = ""
        size_record = {}
        with open(fname, 'r') as csv_file:
            for line in csv_file:
                data = line.strip().split()
                # check if we find the beginning of a module
                if data and data[0] in MBEDTLS_STATIC_LIB:
                    mod = data[0]
                    continue

                if mod:
                    # file_name: SizeEntry(text, data, bss, dec)
                    size_record[data[0]] = CodeSizeGeneratorWithSize.SizeEntry(
                        int(data[1]), int(data[2]), int(data[3]), int(data[4]))

                # check if we hit record for the end of a module
                m = re.match(r'\w+' + self.mod_total_suffix, line)
                if m:
                    if git_rev in self.code_size:
                        self.code_size[git_rev].update({mod: size_record})
                    else:
                        self.code_size[git_rev] = {mod: size_record}
                    mod = ""
                    size_record = {}

    def write_record(
            self,
            git_rev: str,
            code_size_text: typing.Dict[str, str],
            output: typing_util.Writable
    ) -> None:
        """Write size information to a file.

        Writing Format: filename text data bss total(dec)
        """
        for mod, size_text in code_size_text.items():
            self._set_size_record(git_rev, mod, size_text)

        format_string = "{:<30} {:>7} {:>7} {:>7} {:>7}\n"
        output.write(format_string.format("filename",
                                          "text", "data", "bss", "total"))

        for mod, f_size in self.code_size[git_rev].items():
            output.write("\n" + mod + "\n")
            for fname, size_entry in f_size.items():
                output.write(format_string
                             .format(fname,
                                     size_entry.text, size_entry.data,
                                     size_entry.bss, size_entry.total))

    def write_comparison( #pylint: disable=too-many-arguments
            self,
            old_rev: str,
            new_rev: str,
            output: typing_util.Writable,
            with_markdown=False,
            show_all=False
    ) -> None:
        # pylint: disable=too-many-locals
        """Write comparison result into a file.

        Writing Format:
            Markdown Output:
                filename new(text) new(data) change(text) change(data)
            CSV Output:
                filename new(text) new(data) old(text) old(data) change(text) change(data)
        """
        header_line = ["filename", "new(text)", "old(text)", "change(text)",
                       "new(data)", "old(data)", "change(data)"]
        if with_markdown:
            dash_line = [":----", "----:", "----:", "----:",
                         "----:", "----:", "----:"]
            # | filename | new(text) | new(data) | change(text) | change(data) |
            line_format = "| {0:<30} | {1:>9} | {4:>9} | {3:>12} | {6:>12} |\n"
            bold_text = lambda x: '**' + str(x) + '**'
        else:
            # filename new(text) new(data) old(text) old(data) change(text) change(data)
            line_format = "{0:<30} {1:>9} {4:>9} {2:>10} {5:>10} {3:>12} {6:>12}\n"

        def cal_sect_change(
                old_size: typing.Optional[CodeSizeGeneratorWithSize.SizeEntry],
                new_size: typing.Optional[CodeSizeGeneratorWithSize.SizeEntry],
                sect: str
        ) -> typing.List:
            """Inner helper function to calculate size change for a section.

            Convention for special cases:
                - If the object has been removed in new Git revision,
                  the size is minus code size of old Git revision;
                  the size change is marked as `Removed`,
                - If the object only exists in new Git revision,
                  the size is code size of new Git revision;
                  the size change is marked as `None`,

            :param: old_size: code size for objects in old Git revision.
            :param: new_size: code size for objects in new Git revision.
            :param: sect: section to calculate from `size` tool. This could be
                          any instance variable in SizeEntry.
            :return: List of [section size of objects for new Git revision,
                     section size of objects for old Git revision,
                     section size change of objects between two Git revisions]
            """
            if old_size and new_size:
                new_attr = new_size.__dict__[sect]
                old_attr = old_size.__dict__[sect]
                delta = new_attr - old_attr
                change_attr = '{0:{1}}'.format(delta, '+' if delta else '')
            elif old_size:
                new_attr = 'Removed'
                old_attr = old_size.__dict__[sect]
                delta = - old_attr
                change_attr = '{0:{1}}'.format(delta, '+' if delta else '')
            elif new_size:
                new_attr = new_size.__dict__[sect]
                old_attr = 'NotCreated'
                delta = new_attr
                change_attr = '{0:{1}}'.format(delta, '+' if delta else '')
            else:
                # Should never happen
                new_attr = 'Error'
                old_attr = 'Error'
                change_attr = 'Error'
            return [new_attr, old_attr, change_attr]

        # sort dictionary by key
        sort_by_k = lambda item: item[0].lower()
        def get_results(
                f_rev_size:
                typing.Dict[str,
                            typing.Dict[str,
                                        CodeSizeGeneratorWithSize.SizeEntry]]
            ) -> typing.List:
            """Return List of results in the format of:
            [filename, new(text), old(text), change(text),
             new(data), old(data), change(data)]
            """
            res = []
            for fname, revs_size in sorted(f_rev_size.items(), key=sort_by_k):
                old_size = revs_size.get(old_rev)
                new_size = revs_size.get(new_rev)

                text_sect = cal_sect_change(old_size, new_size, 'text')
                data_sect = cal_sect_change(old_size, new_size, 'data')
                # skip the files that haven't changed in code size
                if not show_all and text_sect[-1] == '0' and data_sect[-1] == '0':
                    continue

                res.append([fname, *text_sect, *data_sect])
            return res

        # write header
        output.write(line_format.format(*header_line))
        if with_markdown:
            output.write(line_format.format(*dash_line))
        for mod in MBEDTLS_STATIC_LIB:
        # convert self.code_size to:
        # {
        #   file_name: {
        #       old_rev: SizeEntry,
        #       new_rev: SizeEntry
        #   },
        #   ...
        # }
            f_rev_size = {} #type: typing.Dict[str, typing.Dict]
            for fname, size_entry in self.code_size[old_rev][mod].items():
                f_rev_size.setdefault(fname, {}).update({old_rev: size_entry})
            for fname, size_entry in self.code_size[new_rev][mod].items():
                f_rev_size.setdefault(fname, {}).update({new_rev: size_entry})

            mod_total_sz = f_rev_size.pop(mod + self.mod_total_suffix)
            res = get_results(f_rev_size)
            total_clm = get_results({mod + self.mod_total_suffix: mod_total_sz})
            if with_markdown:
                # bold row of mod-TOTALS in markdown table
                total_clm = [[bold_text(j) for j in i] for i in total_clm]
            res += total_clm

            # write comparison result
            for line in res:
                output.write(line_format.format(*line))


class CodeSizeComparison:
    """Compare code size between two Git revisions."""

    def __init__( #pylint: disable=too-many-arguments
            self,
            old_size_dist_info: CodeSizeDistinctInfo,
            new_size_dist_info: CodeSizeDistinctInfo,
            size_common_info: CodeSizeCommonInfo,
            result_options: CodeSizeResultInfo,
            logger: logging.Logger,
    ) -> None:
        """
        :param old_size_dist_info: CodeSizeDistinctInfo containing old distinct
                                   info to compare code size with.
        :param new_size_dist_info: CodeSizeDistinctInfo containing new distinct
                                   info to take as comparision base.
        :param size_common_info: CodeSizeCommonInfo containing common info for
                                 both old and new size distinct info and
                                 measurement tool.
        :param result_options: CodeSizeResultInfo containing results options for
                               code size record and comparision.
        :param logger: logging module
        """

        self.logger = logger

        self.old_size_dist_info = old_size_dist_info
        self.new_size_dist_info = new_size_dist_info
        self.size_common_info = size_common_info
        # infer pre make command
        self.old_size_dist_info.pre_make_cmd = CodeSizeBuildInfo(
            self.old_size_dist_info, self.size_common_info.host_arch,
            self.logger).infer_pre_make_command()
        self.new_size_dist_info.pre_make_cmd = CodeSizeBuildInfo(
            self.new_size_dist_info, self.size_common_info.host_arch,
            self.logger).infer_pre_make_command()
        # infer make command
        self.old_size_dist_info.make_cmd = CodeSizeBuildInfo(
            self.old_size_dist_info, self.size_common_info.host_arch,
            self.logger).infer_make_command()
        self.new_size_dist_info.make_cmd = CodeSizeBuildInfo(
            self.new_size_dist_info, self.size_common_info.host_arch,
            self.logger).infer_make_command()
        # initialize size parser with corresponding measurement tool
        self.code_size_generator = self.__generate_size_parser()

        self.result_options = result_options
        self.csv_dir = os.path.abspath(self.result_options.record_dir)
        os.makedirs(self.csv_dir, exist_ok=True)
        self.comp_dir = os.path.abspath(self.result_options.comp_dir)
        os.makedirs(self.comp_dir, exist_ok=True)

    def __generate_size_parser(self):
        """Generate a parser for the corresponding measurement tool."""
        if re.match(r'size', self.size_common_info.measure_cmd.strip()):
            return CodeSizeGeneratorWithSize(self.logger)
        else:
            self.logger.error("Unsupported measurement tool: `{}`."
                              .format(self.size_common_info.measure_cmd
                                      .strip().split(' ')[0]))
            sys.exit(1)

    def cal_code_size(
            self,
            size_dist_info: CodeSizeDistinctInfo
        ) -> typing.Dict[str, str]:
        """Calculate code size of library/*.o in a UTF-8 encoding"""

        return CodeSizeCalculator(size_dist_info.git_rev,
                                  size_dist_info.pre_make_cmd,
                                  size_dist_info.make_cmd,
                                  self.size_common_info.measure_cmd,
                                  self.logger).cal_libraries_code_size()

    def gen_code_size_report(self, size_dist_info: CodeSizeDistinctInfo) -> None:
        """Generate code size record and write it into a file."""

        self.logger.info("Start to generate code size record for {}."
                         .format(size_dist_info.git_rev))
        output_file = os.path.join(
            self.csv_dir,
            '{}-{}.csv'
            .format(size_dist_info.get_info_indication(),
                    self.size_common_info.get_info_indication()))
        # Check if the corresponding record exists
        if size_dist_info.git_rev != "current" and \
           os.path.exists(output_file):
            self.logger.debug("Code size csv file for {} already exists."
                              .format(size_dist_info.git_rev))
            self.code_size_generator.read_size_record(
                size_dist_info.git_rev, output_file)
        else:
            # measure code size
            code_size_text = self.cal_code_size(size_dist_info)

            self.logger.debug("Generating code size csv for {}."
                              .format(size_dist_info.git_rev))
            output = open(output_file, "w")
            self.code_size_generator.write_record(
                size_dist_info.git_rev, code_size_text, output)

    def gen_code_size_comparison(self) -> None:
        """Generate results of code size changes between two Git revisions,
        old and new.

        - Measured code size result of these two Git revisions must be available.
        - The result is directed into either file / stdout depending on
          the option, size_common_info.result_options.stdout. (Default: file)
        """

        self.logger.info("Start to generate comparision result between "\
                         "{} and {}."
                         .format(self.old_size_dist_info.git_rev,
                                 self.new_size_dist_info.git_rev))
        if self.result_options.stdout:
            output = sys.stdout
        else:
            output_file = os.path.join(
                self.comp_dir,
                '{}-{}-{}.{}'
                .format(self.old_size_dist_info.get_info_indication(),
                        self.new_size_dist_info.get_info_indication(),
                        self.size_common_info.get_info_indication(),
                        'md' if self.result_options.with_markdown else 'csv'))
            output = open(output_file, "w")

        self.logger.debug("Generating comparison results between {} and {}."
                          .format(self.old_size_dist_info.git_rev,
                                  self.new_size_dist_info.git_rev))
        if self.result_options.with_markdown or self.result_options.stdout:
            print("Measure code size between {} and {} by `{}`."
                  .format(self.old_size_dist_info.get_info_indication(),
                          self.new_size_dist_info.get_info_indication(),
                          self.size_common_info.get_info_indication()),
                  file=output)
        self.code_size_generator.write_comparison(
            self.old_size_dist_info.git_rev,
            self.new_size_dist_info.git_rev,
            output, self.result_options.with_markdown,
            self.result_options.show_all)

    def get_comparision_results(self) -> None:
        """Compare size of library/*.o between self.old_size_dist_info and
        self.old_size_dist_info and generate the result file."""
        build_tree.check_repo_path()
        self.gen_code_size_report(self.old_size_dist_info)
        self.gen_code_size_report(self.new_size_dist_info)
        self.gen_code_size_comparison()

def main():
    parser = argparse.ArgumentParser(description=(__doc__))
    group_required = parser.add_argument_group(
        'required arguments',
        'required arguments to parse for running ' + os.path.basename(__file__))
    group_required.add_argument(
        '-o', '--old-rev', type=str, required=True,
        help='old Git revision for comparison.')

    group_optional = parser.add_argument_group(
        'optional arguments',
        'optional arguments to parse for running ' + os.path.basename(__file__))
    group_optional.add_argument(
        '--record-dir', type=str, default='code_size_records',
        help='directory where code size record is stored. '
             '(Default: code_size_records)')
    group_optional.add_argument(
        '--comp-dir', type=str, default='comparison',
        help='directory where comparison result is stored. '
             '(Default: comparison)')
    group_optional.add_argument(
        '-n', '--new-rev', type=str, default='current',
        help='new Git revision as comparison base. '
             '(Default is the current work directory, including uncommitted '
             'changes.)')
    group_optional.add_argument(
        '-a', '--arch', type=str, default=detect_arch(),
        choices=list(map(lambda s: s.value, SupportedArch)),
        help='Specify architecture for code size comparison. '
             '(Default is the host architecture.)')
    group_optional.add_argument(
        '-c', '--config', type=str, default=SupportedConfig.DEFAULT.value,
        choices=list(map(lambda s: s.value, SupportedConfig)),
        help='Specify configuration type for code size comparison. '
             '(Default is the current Mbed TLS configuration.)')
    group_optional.add_argument(
        '--markdown', action='store_true', dest='markdown',
        help='Show comparision of code size in a markdown table. '
             '(Only show the files that have changed).')
    group_optional.add_argument(
        '--stdout', action='store_true', dest='stdout',
        help='Set this option to direct comparison result into sys.stdout. '
             '(Default: file)')
    group_optional.add_argument(
        '--show-all', action='store_true', dest='show_all',
        help='Show all the objects in comparison result, including the ones '
             'that haven\'t changed in code size. (Default: False)')
    group_optional.add_argument(
        '--verbose', action='store_true', dest='verbose',
        help='Show logs in detail for code size measurement. '
             '(Default: False)')
    comp_args = parser.parse_args()

    logger = logging.getLogger()
    logging_util.configure_logger(logger, split_level=logging.NOTSET)
    logger.setLevel(logging.DEBUG if comp_args.verbose else logging.INFO)

    if os.path.isfile(comp_args.record_dir):
        logger.error("record directory: {} is not a directory"
                     .format(comp_args.record_dir))
        sys.exit(1)
    if os.path.isfile(comp_args.comp_dir):
        logger.error("comparison directory: {} is not a directory"
                     .format(comp_args.comp_dir))
        sys.exit(1)

    comp_args.old_rev = CodeSizeCalculator.validate_git_revision(
        comp_args.old_rev)
    if comp_args.new_rev != 'current':
        comp_args.new_rev = CodeSizeCalculator.validate_git_revision(
            comp_args.new_rev)

    # version, git_rev, arch, config, compiler, opt_level
    old_size_dist_info = CodeSizeDistinctInfo(
        'old', comp_args.old_rev, comp_args.arch, comp_args.config, 'cc', '-Os')
    new_size_dist_info = CodeSizeDistinctInfo(
        'new', comp_args.new_rev, comp_args.arch, comp_args.config, 'cc', '-Os')
    # host_arch, measure_cmd
    size_common_info = CodeSizeCommonInfo(
        detect_arch(), 'size -t')
    # record_dir, comp_dir, with_markdown, stdout, show_all
    result_options = CodeSizeResultInfo(
        comp_args.record_dir, comp_args.comp_dir,
        comp_args.markdown, comp_args.stdout, comp_args.show_all)

    logger.info("Measure code size between {} and {} by `{}`."
                .format(old_size_dist_info.get_info_indication(),
                        new_size_dist_info.get_info_indication(),
                        size_common_info.get_info_indication()))
    CodeSizeComparison(old_size_dist_info, new_size_dist_info,
                       size_common_info, result_options,
                       logger).get_comparision_results()

if __name__ == "__main__":
    main()
