# Cosmopolitan Toolchain

This toolchain can be used to compile executables that run on Linux /
MacOS / Windows / FreeBSD / OpenBSD / NetBSD for both the x86_64 and
AARCH64 architectures. In addition to letting you create portable
binaries, your toolchain is itself comprised of portable binaries,
enabling you to have a consistent development environment that lets you
reach a broader audience from the platform(s) of your choosing.

## What's Included

This toolchain bundles GCC 11.2.0, Cosmopolitan Libc, LLVM LIBCXX, and
LLVM compiler-rt. Additional libraries were provided by Musl Libc, and
the venerable BSDs OSes. This lets you benefit from the awesome modern
GCC compiler with the strongest GPL barrier possible. The preprocessor
advertises cross compilers as both `__COSMOCC__` and `__COSMOPOLITAN__`
whereas `cosmocc` additionally defines `__FATCOSMOCC__`.

The `cosmocc` compiler is designed to generate deterministic output
across platforms. With this release we've confirmed that hello world
binary output is identical on Linux x86+Arm, MacOS x86+Arm, FreeBSD,
OpenBSD, and Windows. Please note that users who need reproducible
builds may also want to look into explicitly defining environment
variables like `LC_ALL=C` and `SOURCE_DATE_EPOCH=0`, in addition to
undefining macros such as `-U__DATE__` and `-U__TIME__`.

## Getting Started

Once your toolchain has been extracted, you can compile hello world:

```
bin/cosmocc -o hello hello.c  # creates multi-os multi-arch binary
```

The `cosmocc` program is shorthand for `unknown-unknown-cosmo-cc`.
For advanced builds it's possible to use `x86_64-unknown-cosmo-cc` and
`aarch64-unknown-cosmo-cc` separately and then join the results together
with the provided `apelink` program. Lastly, the `x86_64-linux-cosmo-cc`
and `aarch64-linux-cosmo-cc` toolchain is the actual physical compiler,
but it's not intended to be called directly. Both cross compilers are
aliases for the `cosmocc` script, which is a thin wrapper around the
Linux toolchain.

## Installation

Your toolchain uses relative paths so it doesn't need to be installed to
any particular system folder, and it needn't be added to your `$PATH`.
There's no external dependencies required to use this toolchain, other
than the UNIX shell.

It's recommended that you install APE loader systemwide, rather than
depending on the APE shell script to self-extract a user's own version.
Apple M1 users should compile `cc -o ape bin/ape-m1.c` and move `ape` to
`/usr/local/bin/ape`. All other platforms use `/usr/bin/ape` as the
canonical path. Linux and BSD users can simply copy `bin/ape.elf` to
`/usr/bin/ape`. MacOS x86-64 users will want `bin/ape.macho`. On Linux,
it's also a good idea to have APE Loader registered with binfmt_misc.

```sh
sudo sh -c "echo ':APE:M::MZqFpD::/usr/bin/ape:' >/proc/sys/fs/binfmt_misc/register"
sudo sh -c "echo ':APE-jart:M::jartsr::/usr/bin/ape:' >/proc/sys/fs/binfmt_misc/register"
sudo sh -c "echo ':qemu-aarch64:M::\x7fELF\x02\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\xb7\x00:\xff\xff\xff\xff\xff\xff\xff\x00\xff\xff\xff\xff\xff\xff\xff\xff\xfe\xff\xff\xff:/usr/bin/qemu-aarch64:CF' >/proc/sys/fs/binfmt_misc/register"
```

Qemu-user is recommended since it's what we've had the most success with
when cross-compiling fat binaries for popular autoconf-based open source
projects. However APE and Cosmo don't depend on it being there.

If you ever need to convert your APE binaries to the platform native
format, this toolchain provides an `assimilate` program which does just
that. Some example use cases would be (1) setuid support, (2) making GDB
less hairy, and (3) code signing. By default, assimilate will choose the
format used by the host system; however it's also possible to explicitly
convert APE programs to any architectures / OS combination. For further
details on usage, run the `assimilate -h` command.

## Gotchas

If you use zsh and have trouble running APE programs try `sh -c ./prog`
or simply upgrade to zsh 5.9+ (since we patched it two years ago). The
same is the case for Python `subprocess`, old versions of fish, etc.

On Apple Silicon, `aarch64-unknown-cosmo-cc` produces ELF binaries. If
you build a hello world program, then you need to say `ape ./hello` to
run it. Note this isn't an issue for `cosmocc` which will wrap the ELF
program in a shell script that'll compile your APE loader automatically
as needed. This also isn't an issue if your login shell was built using
Cosmopolitan Libc, e.g. <https://cosmo.zip/pub/cosmos/bin/bash> because
Cosmo's `execve()` implementation will seamlessly launch ELF files via
your APE Loader.

On Windows, you need a shell in order to run the shell script wrappers
from this toolchain. It's recommended that you download Cosmos binaries
to set up your POSIX userspace. <https://cosmo.zip/pub/cosmos/bin/dash>
could be your `C:\bin\sh` shell (which in Cosmo-speak is `/c/bin/sh`).
The cosmocc shell scripts also depend on programs like `mkdir`, `less`,
`cat`, and `kill` which are available in Cosmos.

When `cosmocc` is run in preprocessor-only mode, it'll use the x86_64
Linux toolchain with macros like `__x86_64__` and `__k8__` undefined.
This might confuse software that's using the C preprocessor to generate
tricked-out assembly instructions. It's not possible to build these
kinds of codebases using `cosmocc` which is just a convenient wrapper
around the cross compilers, which would be a better choice to use in
this type of circumstance.

## About

This toolchain is based on GCC. It's been modified too. We wrote a 2kLOC
patch which gives the C language the ability to `switch (errno) { case
EINVAL: ... }` in cases where constants like `EINVAL` are linkable
symbols. Your code will be rewritten in such cases to use a series of if
statements instead, so that Cosmopolitan Libc's system constants will
work as expected. Our modifications to GNU GCC are published under the
ISC license at <https://github.com/ahgamut/gcc/tree/portcosmo-11.2>. The
binaries you see here were first published at
<https://github.com/ahgamut/superconfigure/releases/tag/z0.0.18> which
is regularly updated.

## Legal

Your Cosmopolitan toolchain is based off Free Software such as GNU GCC.
You have many freedoms to use and modify this software, as described by
the LICENSE files contained within this directory. The software you make
using this toolchain will not be encumbered by the GPL, because we don't
include any GPL licensed headers or runtime libraries. All Cosmopolitan
Libc runtime libraries are exclusively under permissive notice licenses,
e.g. ISC, MIT, BSD, etc. There are many copyright notices with the names
of people who've helped build your toolchain. You have an obligation to
distribute those notices along with your binaries. Cosmopolitan makes
that easy. Your C library is configured to use `.ident` directives to
ensure the relevant notices are automatically embedded within your
binaries. You can view them using tools like `less <bin/foo`.

## Contact

For further questions and inquiries regarding this toolchain, feel free
to contact Justine Tunney <jtunney@gmail.com>.

## See Also

- <https://cosmo.zip/> for downloadable binaries built with cosmocc
- <https://github.com/ahgamut/superconfigure/> for cosmocc build recipes
