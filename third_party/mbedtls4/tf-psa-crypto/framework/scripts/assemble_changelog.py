#!/usr/bin/env python3

"""Assemble Mbed TLS change log entries into the change log file.

Add changelog entries to the first level-2 section.
Create a new level-2 section for unreleased changes if needed.
Remove the input files unless --keep-entries is specified.

In each level-3 section, entries are sorted in chronological order
(oldest first). From oldest to newest:
* Merged entry files are sorted according to their merge date (date of
  the merge commit that brought the commit that created the file into
  the target branch).
* Committed but unmerged entry files are sorted according to the date
  of the commit that adds them.
* Uncommitted entry files are sorted according to their modification time.

You must run this program from within a git working directory.
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

import argparse
from collections import OrderedDict, namedtuple
import datetime
import functools
import glob
import os
import re
import subprocess
import sys

class InputFormatError(Exception):
    def __init__(self, filename, line_number, message, *args, **kwargs):
        message = '{}:{}: {}'.format(filename, line_number,
                                     message.format(*args, **kwargs))
        super().__init__(message)

class CategoryParseError(Exception):
    def __init__(self, line_offset, error_message):
        self.line_offset = line_offset
        self.error_message = error_message
        super().__init__('{}: {}'.format(line_offset, error_message))

class LostContent(Exception):
    def __init__(self, filename, line):
        message = ('Lost content from {}: "{}"'.format(filename, line))
        super().__init__(message)

class FilePathError(Exception):
    def __init__(self, filenames):
        message = ('Changelog filenames do not end with .txt: {}'.format(", ".join(filenames)))
        super().__init__(message)

# The category names we use in the changelog.
# If you edit this, update ChangeLog.d/README.md.
STANDARD_CATEGORIES = (
    'API changes',
    'Default behavior changes',
    'Requirement changes',
    'New deprecations',
    'Removals',
    'Features',
    'Security',
    'Bugfix',
    'Changes',
)

# The maximum line length for an entry
MAX_LINE_LENGTH = 80

CategoryContent = namedtuple('CategoryContent', [
    'name', 'title_line', # Title text and line number of the title
    'body', 'body_line', # Body text and starting line number of the body
])

class ChangelogFormat:
    """Virtual class documenting how to write a changelog format class."""

    @classmethod
    def extract_top_version(cls, changelog_file_content):
        """Split out the top version section.

        If the top version is already released, create a new top
        version section for an unreleased version.

        Return ``(header, top_version_title, top_version_body, trailer)``
        where the "top version" is the existing top version section if it's
        for unreleased changes, and a newly created section otherwise.
        To assemble the changelog after modifying top_version_body,
        concatenate the four pieces.
        """
        raise NotImplementedError

    @classmethod
    def split_categories(cls, version_body):
        """Split a changelog version section body into categories.

        Return a list of `CategoryContent` the name is category title
        without any formatting.
        """
        raise NotImplementedError

    @classmethod
    def format_category(cls, title, body):
        """Construct the text of a category section from its title and body."""
        raise NotImplementedError

class TextChangelogFormat(ChangelogFormat):
    """The traditional Mbed TLS changelog format."""

    _unreleased_version_text = '= {} x.x.x branch released xxxx-xx-xx'
    @classmethod
    def is_released_version(cls, title):
        # Look for an incomplete release date
        return not re.search(r'[0-9x]{4}-[0-9x]{2}-[0-9x]?x', title)

    _top_version_re = re.compile(r'(?:\A|\n)(=[^\n]*\n+)(.*?\n)(?:=|$)',
                                 re.DOTALL)
    _name_re = re.compile(r'=\s(.*)\s[0-9x]+\.', re.DOTALL)
    @classmethod
    def extract_top_version(cls, changelog_file_content):
        """A version section starts with a line starting with '='."""
        m = re.search(cls._top_version_re, changelog_file_content)
        top_version_start = m.start(1)
        top_version_end = m.end(2)
        top_version_title = m.group(1)
        top_version_body = m.group(2)
        name = re.match(cls._name_re, top_version_title).group(1)
        if cls.is_released_version(top_version_title):
            top_version_end = top_version_start
            top_version_title = cls._unreleased_version_text.format(name) + '\n\n'
            top_version_body = ''
        return (changelog_file_content[:top_version_start],
                top_version_title, top_version_body,
                changelog_file_content[top_version_end:])

    _category_title_re = re.compile(r'(^\w.*)\n+', re.MULTILINE)
    @classmethod
    def split_categories(cls, version_body):
        """A category title is a line with the title in column 0."""
        if not version_body:
            return []
        title_matches = list(re.finditer(cls._category_title_re, version_body))
        if not title_matches or title_matches[0].start() != 0:
            # There is junk before the first category.
            raise CategoryParseError(0, 'Junk found where category expected')
        title_starts = [m.start(1) for m in title_matches]
        body_starts = [m.end(0) for m in title_matches]
        body_ends = title_starts[1:] + [len(version_body)]
        bodies = [version_body[body_start:body_end].rstrip('\n') + '\n'
                  for (body_start, body_end) in zip(body_starts, body_ends)]
        title_lines = [version_body[:pos].count('\n') for pos in title_starts]
        body_lines = [version_body[:pos].count('\n') for pos in body_starts]
        return [CategoryContent(title_match.group(1), title_line,
                                body, body_line)
                for title_match, title_line, body, body_line
                in zip(title_matches, title_lines, bodies, body_lines)]

    @classmethod
    def format_category(cls, title, body):
        # `split_categories` ensures that each body ends with a newline.
        # Make sure that there is additionally a blank line between categories.
        if not body.endswith('\n\n'):
            body += '\n'
        return title + '\n' + body

class ChangeLog:
    """An Mbed TLS changelog.

    A changelog file consists of some header text followed by one or
    more version sections. The version sections are in reverse
    chronological order. Each version section consists of a title and a body.

    The body of a version section consists of zero or more category
    subsections. Each category subsection consists of a title and a body.

    A changelog entry file has the same format as the body of a version section.

    A `ChangelogFormat` object defines the concrete syntax of the changelog.
    Entry files must have the same format as the changelog file.
    """

    # Only accept dotted version numbers (e.g. "3.1", not "3").
    # Refuse ".x" in a version number where x is a letter: this indicates
    # a version that is not yet released. Something like "3.1a" is accepted.
    _version_number_re = re.compile(r'[0-9]+\.[0-9A-Za-z.]+')
    _incomplete_version_number_re = re.compile(r'.*\.[A-Za-z]')
    _only_url_re = re.compile(r'^\s*\w+://\S+\s*$')
    _has_url_re = re.compile(r'.*://.*')

    def add_categories_from_text(self, filename, line_offset,
                                 text, allow_unknown_category):
        """Parse a version section or entry file."""
        try:
            categories = self.format.split_categories(text)
        except CategoryParseError as e:
            raise InputFormatError(filename, line_offset + e.line_offset,
                                   e.error_message)
        for category in categories:
            if not allow_unknown_category and \
               category.name not in self.categories:
                raise InputFormatError(filename,
                                       line_offset + category.title_line,
                                       'Unknown category: "{}"',
                                       category.name)

            body_split = category.body.splitlines()

            for line_number, line in enumerate(body_split, 1):
                if not self._only_url_re.match(line) and \
                   len(line) > MAX_LINE_LENGTH:
                    long_url_msg = '. URL exceeding length limit must be alone in its line.' \
                        if self._has_url_re.match(line) else ""
                    raise InputFormatError(filename,
                                           category.body_line + line_number,
                                           'Line is longer than allowed: '
                                           'Length {} (Max {}){}',
                                           len(line), MAX_LINE_LENGTH,
                                           long_url_msg)

            self.categories[category.name] += category.body

    def __init__(self, input_stream, changelog_format):
        """Create a changelog object.

        Populate the changelog object from the content of the file
        input_stream.
        """
        self.format = changelog_format
        whole_file = input_stream.read()
        (self.header,
         self.top_version_title, top_version_body,
         self.trailer) = self.format.extract_top_version(whole_file)
        # Split the top version section into categories.
        self.categories = OrderedDict()
        for category in STANDARD_CATEGORIES:
            self.categories[category] = ''
        offset = (self.header + self.top_version_title).count('\n') + 1

        self.add_categories_from_text(input_stream.name, offset,
                                      top_version_body, True)

    def add_file(self, input_stream):
        """Add changelog entries from a file.
        """
        self.add_categories_from_text(input_stream.name, 1,
                                      input_stream.read(), False)

    def write(self, filename):
        """Write the changelog to the specified file.
        """
        with open(filename, 'w', encoding='utf-8') as out:
            out.write(self.header)
            out.write(self.top_version_title)
            for title, body in self.categories.items():
                if not body:
                    continue
                out.write(self.format.format_category(title, body))
            out.write(self.trailer)


@functools.total_ordering
class EntryFileSortKey:
    """This classes defines an ordering on changelog entry files: older < newer.

    * Merged entry files are sorted according to their merge date (date of
      the merge commit that brought the commit that created the file into
      the target branch).
    * Committed but unmerged entry files are sorted according to the date
      of the commit that adds them.
    * Uncommitted entry files are sorted according to their modification time.

    This class assumes that the file is in a git working directory with
    the target branch checked out.
    """

    # Categories of files. A lower number is considered older.
    MERGED = 0
    COMMITTED = 1
    LOCAL = 2

    @staticmethod
    def creation_hash(filename):
        """Return the git commit id at which the given file was created.

        Return None if the file was never checked into git.
        """
        hashes = subprocess.check_output(['git', 'log', '--format=%H',
                                          '--follow',
                                          '--', filename])
        m = re.search('(.+)$', hashes.decode('ascii'))
        if not m:
            # The git output is empty. This means that the file was
            # never checked in.
            return None
        # The last commit in the log is the oldest one, which is when the
        # file was created.
        return m.group(0)

    @staticmethod
    def list_merges(some_hash, target, *options):
        """List merge commits from some_hash to target.

        Pass options to git to select which commits are included.
        """
        text = subprocess.check_output(['git', 'rev-list',
                                        '--merges', *options,
                                        '..'.join([some_hash, target])])
        return text.decode('ascii').rstrip('\n').split('\n')

    @classmethod
    def merge_hash(cls, some_hash):
        """Return the git commit id at which the given commit was merged.

        Return None if the given commit was never merged.
        """
        target = 'HEAD'
        # List the merges from some_hash to the target in two ways.
        # The ancestry list is the ones that are both descendants of
        # some_hash and ancestors of the target.
        ancestry = frozenset(cls.list_merges(some_hash, target,
                                             '--ancestry-path'))
        # The first_parents list only contains merges that are directly
        # on the target branch. We want it in reverse order (oldest first).
        first_parents = cls.list_merges(some_hash, target,
                                        '--first-parent', '--reverse')
        # Look for the oldest merge commit that's both on the direct path
        # and directly on the target branch. That's the place where some_hash
        # was merged on the target branch. See
        # https://stackoverflow.com/questions/8475448/find-merge-commit-which-include-a-specific-commit
        for commit in first_parents:
            if commit in ancestry:
                return commit
        return None

    @staticmethod
    def commit_timestamp(commit_id):
        """Return the timestamp of the given commit."""
        text = subprocess.check_output(['git', 'show', '-s',
                                        '--format=%ct',
                                        commit_id])
        return datetime.datetime.utcfromtimestamp(int(text))

    @staticmethod
    def file_timestamp(filename):
        """Return the modification timestamp of the given file."""
        mtime = os.stat(filename).st_mtime
        return datetime.datetime.fromtimestamp(mtime)

    def __init__(self, filename):
        """Determine position of the file in the changelog entry order.

        This constructor returns an object that can be used with comparison
        operators, with `sort` and `sorted`, etc. Older entries are sorted
        before newer entries.
        """
        self.filename = filename
        creation_hash = self.creation_hash(filename)
        if not creation_hash:
            self.category = self.LOCAL
            self.datetime = self.file_timestamp(filename)
            return
        merge_hash = self.merge_hash(creation_hash)
        if not merge_hash:
            self.category = self.COMMITTED
            self.datetime = self.commit_timestamp(creation_hash)
            return
        self.category = self.MERGED
        self.datetime = self.commit_timestamp(merge_hash)

    def sort_key(self):
        """"Return a concrete sort key for this entry file sort key object.

        ``ts1 < ts2`` is implemented as ``ts1.sort_key() < ts2.sort_key()``.
        """
        return (self.category, self.datetime, self.filename)

    def __eq__(self, other):
        return self.sort_key() == other.sort_key()

    def __lt__(self, other):
        return self.sort_key() < other.sort_key()


def check_output(generated_output_file, main_input_file, merged_files):
    """Make sanity checks on the generated output.

    The intent of these sanity checks is to have reasonable confidence
    that no content has been lost.

    The sanity check is that every line that is present in an input file
    is also present in an output file. This is not perfect but good enough
    for now.
    """
    with open(generated_output_file, 'r', encoding='utf-8') as fd:
        generated_output = set(fd)
        for line in open(main_input_file, 'r', encoding='utf-8'):
            if line not in generated_output:
                raise LostContent('original file', line)
        for merged_file in merged_files:
            for line in open(merged_file, 'r', encoding='utf-8'):
                if line not in generated_output:
                    raise LostContent(merged_file, line)

def finish_output(changelog, output_file, input_file, merged_files):
    """Write the changelog to the output file.

    The input file and the list of merged files are used only for sanity
    checks on the output.
    """
    if os.path.exists(output_file) and not os.path.isfile(output_file):
        # The output is a non-regular file (e.g. pipe). Write to it directly.
        output_temp = output_file
    else:
        # The output is a regular file. Write to a temporary file,
        # then move it into place atomically.
        output_temp = output_file + '.tmp'
    changelog.write(output_temp)
    check_output(output_temp, input_file, merged_files)
    if output_temp != output_file:
        os.rename(output_temp, output_file)

def remove_merged_entries(files_to_remove):
    for filename in files_to_remove:
        os.remove(filename)

def list_files_to_merge(options):
    """List the entry files to merge, oldest first.

    "Oldest" is defined by `EntryFileSortKey`.

    Also check for required .txt extension
    """
    files_to_merge = glob.glob(os.path.join(options.dir, '*'))

    # Ignore 00README.md
    readme = os.path.join(options.dir, "00README.md")
    if readme in files_to_merge:
        files_to_merge.remove(readme)

    # Identify files without the required .txt extension
    bad_files = [x for x in files_to_merge if not x.endswith(".txt")]
    if bad_files:
        raise FilePathError(bad_files)

    files_to_merge.sort(key=EntryFileSortKey)
    return files_to_merge

def merge_entries(options):
    """Merge changelog entries into the changelog file.

    Read the changelog file from options.input.
    Check that all entries have a .txt extension
    Read entries to merge from the directory options.dir.
    Write the new changelog to options.output.
    Remove the merged entries if options.keep_entries is false.
    """
    with open(options.input, 'r', encoding='utf-8') as input_file:
        changelog = ChangeLog(input_file, TextChangelogFormat)
    files_to_merge = list_files_to_merge(options)
    if not files_to_merge:
        sys.stderr.write('There are no pending changelog entries.\n')
        return
    for filename in files_to_merge:
        with open(filename, 'r', encoding='utf-8') as input_file:
            changelog.add_file(input_file)
    finish_output(changelog, options.output, options.input, files_to_merge)
    if not options.keep_entries:
        remove_merged_entries(files_to_merge)

def show_file_timestamps(options):
    """List the files to merge and their timestamp.

    This is only intended for debugging purposes.
    """
    files = list_files_to_merge(options)
    for filename in files:
        ts = EntryFileSortKey(filename)
        print(ts.category, ts.datetime, filename)

def set_defaults(options):
    """Add default values for missing options."""
    output_file = getattr(options, 'output', None)
    if output_file is None:
        options.output = options.input
    if getattr(options, 'keep_entries', None) is None:
        options.keep_entries = (output_file is not None)

def main():
    """Command line entry point."""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--dir', '-d', metavar='DIR',
                        default='ChangeLog.d',
                        help='Directory to read entries from'
                             ' (default: ChangeLog.d)')
    parser.add_argument('--input', '-i', metavar='FILE',
                        default='ChangeLog',
                        help='Existing changelog file to read from and augment'
                             ' (default: ChangeLog)')
    parser.add_argument('--keep-entries',
                        action='store_true', dest='keep_entries', default=None,
                        help='Keep the files containing entries'
                             ' (default: remove them if --output/-o is not specified)')
    parser.add_argument('--no-keep-entries',
                        action='store_false', dest='keep_entries',
                        help='Remove the files containing entries after they are merged'
                             ' (default: remove them if --output/-o is not specified)')
    parser.add_argument('--output', '-o', metavar='FILE',
                        help='Output changelog file'
                             ' (default: overwrite the input)')
    parser.add_argument('--list-files-only',
                        action='store_true',
                        help=('Only list the files that would be processed '
                              '(with some debugging information)'))
    options = parser.parse_args()
    set_defaults(options)
    if options.list_files_only:
        show_file_timestamps(options)
        return
    merge_entries(options)

if __name__ == '__main__':
    main()
