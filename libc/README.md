# Cosmopolitan Standard Library

This directory defines static archives defining functions, like
`printf()`, `mmap()`, win32, etc. Please note that the Cosmopolitan
build configuration doesn't link any C/C++ library dependencies
by default, so you still have the flexibility to choose the one
provided by your system. If you'd prefer Cosmopolitan, just add
`$(LIBC)` and `$(CRT)` to your linker arguments.

Your library is compromised of many bite-sized static archives.
We use the checkdeps tool to guarantee that the contents of the
archives are organized in a logical way that's easy to use with
or without our makefile infrastructure, since there's no cyclic
dependencies.

The Cosmopolitan Library exports only the most stable canonical
system calls for all supported operating systems, regardless of
which platform is used for compilation. We polyfill many of the
APIs, e.g. `read()`, `write()` so they work consistently everywhere
while other apis, e.g. `CreateWindowEx()`, might only work on one
platform, in which case they become no-op functions on others.

Cosmopolitan polyfill wrappers will usually use the dollar sign
naming convention, so they may be bypassed when necessary. This
same convention is used when multiple implementations of string
library and other performance-critical function are provided to
allow Cosmopolitan to go fast on both old and newer computers.

We take an approach to configuration that relies heavily on the
compiler's dead code elimination pass (`libc/dce.h`). Most of the
code is written so that, for example, folks not wanting support
for OpenBSD can flip a bit in `SUPPORT_VECTOR` and that code will
be omitted from the build. The same is true for builds that are
tuned using `-march=native` which effectively asks the library to
not include runtime support hooks for x86 processors older than
what you use.

Please note that, unlike Cygwin or MinGW, Cosmopolitan does not
achieve broad support by bolting on a POSIX emulation layer. We
do nothing more than (in most cases) stateless API translations
that get you 90% of the way there in a fast lightweight manner.
We therefore can't address some of the subtle differences, such
as the nuances of absolute paths on Windows. Our approach could
be compared to something more along the lines of, "the Russians
just used a pencil to write in space", versus spending millions
researching a pen like NASA.
