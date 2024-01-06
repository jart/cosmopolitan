# Contributing Guidelines

We'd love to accept your patches! Please read this guide first.

## Identity Disclosure

This project does not accept anonymous contributions. Justine Tunney
won't merge pull requests from strangers. In order to change the Cosmo
codebase, and have your changes be upstreamed, she has to know who you
are. You're encouraged to disclose your full name and email address to
the public too, by including them in your git commit messages; however
that's not a requirement; as we're happy to respect the wishes of
contributors who prefer to remain anonymous to the public.

## Copyright Assignment

The first time you send a pull request, you need to send an email to
Justine Tunney <jtunney@gmail.com> stating that you intend to assign her
the copyright to the changes you contribute to Cosmopolitan. This only
applies to the code you *choose* to contribute. It only has to happen
once. The email should be sent from an email address associated with
your identity. Your email should link to your pull request.

Please note that in order to give Justine the copyright, it has to be
yours to give in the first place. If you're employed, then you should
get your employer's approval to do this beforehand. Even with big
companies like Google, this process is quick and painless. Usually we
see employers granting authorization in less than one day.

If you live in a country that doesn't recognize one's ability to assign
copyright, then you may alternatively consider disclaiming it using the
language in [Unlicense](https://unlicense.org) or
[CC-0](http://creativecommons.org/share-your-work/public-domain/cc0).

If you're checking-in third party code, then you need to have headers at
the top of each source file (but never header files) documenting its
owners and the code should go in the `third_party/` folder. Every third
party project should have a `README.cosmo` file that documents its
provenance as well as any local changes you've made.

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
