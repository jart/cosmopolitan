# Pending changelog entry directory

This directory contains changelog entries that have not yet been merged
to the changelog file ([`../ChangeLog`](../ChangeLog)).

## What requires a changelog entry?

Write a changelog entry if there is a user-visible change. This includes:

* Bug fixes in the library or in sample programs: fixing a security hole,
  fixing broken behavior, fixing the build in some configuration or on some
  platform, etc.
* New features in the library, new sample programs, or new platform support.
* Changes in existing behavior. These should be rare. Changes in features
  that are documented as experimental may or may not be announced, depending
  on the extent of the change and how widely we expect the feature to be used.

We generally don't include changelog entries for:

* Documentation improvements.
* Performance improvements, unless they are particularly significant.
* Changes to parts of the code base that users don't interact with directly,
  such as test code and test data.
* Fixes for compiler warnings. Releases typically contain a number of fixes
  of this kind, so we will only mention them in the Changelog if they are
  particularly significant.

Looking at older changelog entries is good practice for how to write a
changelog entry, but not for deciding whether to write one.

## Changelog entry file format

A changelog entry file must have the extension `*.txt` and must have the
following format:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Security
   * Change description.
   * Another change description.

Features
   * Yet another change description. This is a long change description that
     spans multiple lines.
   * Yet again another change description.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The permitted changelog entry categories are as follows:
<!-- Keep this synchronized with STANDARD_CATEGORIES in assemble_changelog.py! -->

    API changes
    Default behavior changes
    Requirement changes
    New deprecations
    Removals
    Features
    Security
    Bugfix
    Changes

Use “Changes” for anything that doesn't fit in the other categories.

## How to write a changelog entry

Each entry starts with three spaces, an asterisk and a space. Continuation
lines start with 5 spaces. Lines wrap at 79 characters.

Write full English sentences with proper capitalization and punctuation. Use
the present tense. Use the imperative where applicable. For example: “Fix a
bug in mbedtls_xxx() ….”

Include GitHub issue numbers where relevant. Use the format “#1234” for a
TF-PSA-Crypto issue. Add other external references such as CVE numbers where
applicable.

Credit bug reporters where applicable.

**Explain why, not how**. Remember that the audience is the users of the
library, not its developers. In particular, for a bug fix, explain the
consequences of the bug, not how the bug was fixed. For a new feature, explain
why one might be interested in the feature. For an API change or a deprecation,
explain how to update existing applications.

See [existing entries](../ChangeLog) for examples.

## How `ChangeLog` is updated

Run [`../scripts/assemble_changelog.py`](../scripts/assemble_changelog.py)
from a Git working copy
to move the entries from files in `ChangeLog.d` to the main `ChangeLog` file.
