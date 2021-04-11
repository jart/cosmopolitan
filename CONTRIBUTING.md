# Contributing Guidelines

We'd love to accept your patches! Please read this guide first.

## Copyright Assignment

Please send an email to Justine Tunney <jtunney@gmail.com> stating that
you intend to assign her the copyright to the changes you contribute to
Cosmopolitan. Please use the same email address you use for git commits
which should only contain original source code from you or other people
who are also assigning copyright. Please note that, if you're employed,
you may need to get your employer's approval beforehand. If you can not
assign copyright due to local laws, then you may alternatively consider
disclaiming it using the language in [Unlicense](https://unlicense.org)
or [CC-0](http://creativecommons.org/share-your-work/public-domain/cc0)

This is important because we can't produce 12kb single-file executables
that comply with license requirements if we have to embed lots of them.
Although that's less of an issue depending on the purpose of the files.
For example, ownership is much less of a concern in the unit test files
so you're encouraged to put your copyright on those, provided it's ISC.

## Style Guide

You can use clang-format to automatically format your files:

```sh
clang-format -i -style=file tool/net/redbean.c
```

If you use Emacs this can be automated on save for Cosmopolitan using
[tool/emacs/cosmo-format.el]([tool/emacs/cosmo-format.el]).

### Source Files

- Must use include paths relative to the root of the repository
- Must have comment at top of file documenting copyright and license
- Must have notice embedding if not owned by Justine (exception: tests)
- May use language extensions that are supported by both GCC and Clang
- Should use Google indentation (otherwise use `/* clang-format off */`)
- Should use asm() instead of compiler APIs (exception: ctz, clz, memcpy)

### Header Files

- Must not have copyright or license comments
- Must have once guards (otherwise change `.h` to `.inc`)
- Must be ANSI C89 compatible to be included in the amalgamation header
- Must include its dependencies (exception: libc/integral/normalize.inc)
- Must not define objects (i.e. `cc -c -xc foo.h` will produce empty `.o`)
- Should not use typedefs
- Should not use forward declarations
- Should not include documentation comments
- Should not include parameter names in prototypes
- Should not pose problems if included by C++ or Assembly sources
- Should not declare non-ANSI code, at all, when the user requests ANSI

### Build Config

- Must not write files outside `o/`
- Must not communicate with Internet
- Must not depend on system libraries
- Must not depend on system commands (exception: sh, make, gzip, zip)
