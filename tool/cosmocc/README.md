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

## Getting Started

Once your toolchain has been extracted, you can compile hello world:

```
bin/cosmocc -o hello hello.c  # creates multi-os multi-arch binary
```

You now have an [actually portable
executable](https://justine.lol/ape.html) that'll run on your host
system. If anything goes wrong, see the Gotchas and Troubleshoot
sections below. It should have also outputted two ELF executables as
well, named `hello.com.dbg` (x86-64 Linux ELF) and `hello.aarch64.elf`
(AARCH64 Linux ELF). On Linux systems, those files are also runnable,
which is useful for easily running programs in GDB. On other OSes GDB
can still debug APE programs if the ELF is loaded in a second step using
the `add-symbol-file` command.

## Overview

The `cosmocc` program is shorthand for `unknown-unknown-cosmo-cc`. For
advanced builds it's possible to use `x86_64-unknown-cosmo-cc` and
`aarch64-unknown-cosmo-cc` separately and then join the results together
with the provided `apelink` program. Lastly, the `x86_64-linux-cosmo-cc`
and `aarch64-linux-cosmo-cc` toolchain is the actual physical compiler,
which isn't intended to be called directly (unless one's goal is maximum
configurability or a freestanding environment).

The `cosmocc` compiler is designed to generate deterministic output
across platforms. With this release we've confirmed that hello world
binary output is identical on Linux x86+Arm, MacOS x86+Arm, FreeBSD,
OpenBSD, and Windows. Please note that users who need reproducible
builds may also want to look into explicitly defining environment
variables like `LC_ALL=C` and `SOURCE_DATE_EPOCH=0`, in addition to
undefining macros such as `-U__DATE__` and `-U__TIME__`.

## Installation

Your toolchain uses relative paths so it doesn't need to be installed to
any particular system folder, and it needn't be added to your `$PATH`.
There's no external dependencies required to use this toolchain, other
than the UNIX shell.

It's recommended that the APE Loader be installed systemwide, rather
than depending on the default behavior of the APE shell script, which is
to self-extract an APE loader to each user's `$TMPDIR` or `$HOME`. Apple
Arm64 users should compile `cc -O -o ape bin/ape-m1.c` and move `ape` to
`/usr/local/bin/ape`. All other platforms use `/usr/bin/ape` as the
canonical path. Linux and BSD users can simply copy `bin/ape.elf` to
`/usr/bin/ape`. MacOS x86-64 users will want `bin/ape.macho`. On Linux,
it's possible to have APE executables run 400 microseconds faster by
registering APE with binfmt_misc.

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

If you're on Linux, then `binfmt_misc` might try to run APE programs
under WINE, or say "run-detectors: unable to find an interpreter". You
can fix that by running these commands:

```sh
sudo wget -O /usr/bin/ape https://cosmo.zip/pub/cosmos/bin/ape-$(uname -m).elf
sudo chmod +x /usr/bin/ape
sudo sh -c "echo ':APE:M::MZqFpD::/usr/bin/ape:' >/proc/sys/fs/binfmt_misc/register"
sudo sh -c "echo ':APE-jart:M::jartsr::/usr/bin/ape:' >/proc/sys/fs/binfmt_misc/register"
```

On Apple Silicon, `aarch64-unknown-cosmo-cc` produces ELF binaries. If
you build a hello world program, then you need to say `ape ./hello`. If
you don't have an `ape` command then run `cc -o ape bin/ape-m1.c` which
should be moved to `/usr/local/bin/ape`. Your APE interpreter might
already exist under a path like `$TMPDIR/.ape-1.10`. It's important to
note this is only a gotcha for the cross compiler. Your `cosmocc`
compiler wraps the actual ELF binaries with a shell script that'll
extract and compile an APE loader automatically, as needed. This also
isn't an issue if your login shell was built using Cosmopolitan Libc,
e.g. <https://cosmo.zip/pub/cosmos/bin/bash>. That's because Cosmo's
`execve()` implementation will automatically react to `ENOEXEC` from the
kernel by re-launching the program under `/usr/local/bin/ape`. Lastly
note that all other platforms that aren't Apple Arm64 use `/usr/bin/ape`
as the hard-coded canonical interpreter path.

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

## Usage

By default, all the code you compile will use the baseline of the X86_64
and AARCH64, which is K8 and ARMv8.0. You can pass architecture specific
flags to use newer ISAs by using the `-Xx86_64` and `-Xaarch64` prefixes
like `-Xx86_64-mssse3` and `-Xaarch64-march=armv8.2-a+dotprod`.

## Troubleshooting

Your `cosmocc` compiler runs a number commands under the hood. If
something goes wrong, you can gain more visibility into its process by
setting the `BUILDLOG` environment variable.

```sh
export BUILDLOG=log
bin/cosmocc -o hello hello.c
```

The log will then contain a log of commands you can copy and paste into
your shell to reproduce the build process, or simply see what flags are
being passed to the freestanding Linux compiler.

```sh
# bin/cosmocc -o hello hello.c
(cd /home/jart/cosmocc; bin/x86_64-linux-cosmo-gcc -o/tmp/fatcosmocc.i5lugr6bc0gu0.o -D__COSMOPOL...
(cd /home/jart/cosmocc; bin/aarch64-linux-cosmo-gcc -o/tmp/fatcosmocc.w48k03qgw8692.o -D__COSMOPO...
(cd /home/jart/cosmocc; bin/fixupobj /tmp/fatcosmocc.i5lugr6bc0gu0.o)
(cd /home/jart/cosmocc; bin/fixupobj /tmp/fatcosmocc.w48k03qgw8692.o)
(cd /home/jart/cosmocc; bin/x86_64-linux-cosmo-gcc -o/tmp/fatcosmocc.ovdo2nqvkjjg3.com.dbg c...
(cd /home/jart/cosmocc; bin/aarch64-linux-cosmo-gcc -o/tmp/fatcosmocc.d3ca1smuot0k0.aarch64.elf /...
(cd /home/jart/cosmocc; bin/fixupobj /tmp/fatcosmocc.d3ca1smuot0k0.aarch64.elf)
(cd /home/jart/cosmocc; bin/fixupobj /tmp/fatcosmocc.ovdo2nqvkjjg3.com.dbg)
(cd /home/jart/cosmocc; bin/apelink -l bin/ape.elf -l bin/ape.aarch64 -...
(cd /home/jart/cosmocc; bin/pecheck hello)
```

## Building Open Source Software

Assuming you put `cosmocc/bin/` on your `$PATH`, integrating with GNU
Autotools projects becomes easy. The trick here is to use a `--prefix`
that *only* contains software that's been built by cosmocc. That's
because Cosmopolitan Libc uses a different ABI than your distro.

```sh
export CC="cosmocc -I/opt/cosmos/include -L/opt/cosmos/lib"
export CXX="cosmoc++ -I/opt/cosmos/include -L/opt/cosmos/lib"
export INSTALL=cosmoinstall
export AR=cosmoar
./configure --prefix=/opt/cosmos
make -j
make install
```

## Tools

While the GNU GCC and Binutils programs included in your `cosmocc`
toolchain require no explanation, other programs are included that many
users might not be familiar with.

### `assimilate`

The `assimilate` program may be used to convert actually portable
executables into native executables. By default, this tool converts to
the format used by the host operating system and architecture. However
flags may be passed to convert APE binaries for foreign platforms too.

### `ctags`

The `ctags` program is exuberant-ctags 1:5.9~svn20110310-14 built from
the Cosmopolitan Libc third_party sources. It may be used to generate an
index of symbols for your your text editor that enables easy source code
navigation.

### `apelink`

The `apelink` program is the actually portable executable linker. It
accepts as input (1) multiple executables that were linked by GNU
ld.bfd, (2) the paths of native APE Loader executables for ELF
platforms, and (3) the source code for the Apple Silicon APE loader. It
then weaves them all together into a shell script that self-extracts the
appropriate tiny ~10kb APE Loader, when is then re-exec'd to map the
bulk of the appropriate embedded executable into memory.

### `mkdeps`

The `mkdeps` program can be used to generate a deps file for your
Makefile, which declares which source files include which headers. This
command is impressively fast. Much more so than relying on `gcc -MMD`.
This was originally built for the Cosmopolitan Libc repository, which
has ~10,000 source files. Using `mkdeps`, Cosmo is able to generate an
`o//depend` file with ~100,000 lines in ~70 milliseconds.

It can be used by adding something like this to your `Makefile`.

```make
FILES := $(wildcard src/*)
SRCS = $(filter %.c,$(FILES))
HDRS = $(filter %.h,$(FILES))

o/$(MODE)/depend: $(SRCS) $(HDRS)
	@mkdir -o $(@D)
	mkdeps -o $@ -r o/$(MODE)/ $(SRCS) $(HDRS)

$(SRCS):
$(HDRS):
.DEFAULT:
	@echo
	@echo NOTE: deleting o/$(MODE)/depend because of an unspecified prerequisite: $@
	@echo
	rm -f o/$(MODE)/depend

-include o/$(MODE)/depend
```

If your project is very large like Cosmopolitan, then `mkdeps` supports
arguments files. That's particularly helpful on Windows, which has a
32768 character limit on command arguments.

```make
SRCS = $(foreach x,$(PKGS),$($(x)_SRCS))
HDRS = $(foreach x,$(PKGS),$($(x)_HDRS))

o/$(MODE)/depend: $(SRCS) $(HDRS)
	$(file >$@.args,$(SRCS) $(HDRS))
	@mkdir -o $(@D)
	mkdeps -o $@ -r o/$(MODE)/ @$@.args
```

### `cosmoaddr2line`

The `cosmoaddr2line` program may be used to print backtraces, based on
DWARF data, whenever one of your programs reports a crash. It accepts as
an argument the ELF executable produced by `cosmocc`, which is different
from the APE executable. For example, if `cosmocc` compiles a program
named `hello` then you'll need to pass either `hello.com.dbg` (x86-64)
or `hello.aarch64.elf` to cosmoaddr2line to get the backtrace. After the
ELf executable comes the program counter (instruction pointer) addresses
which are easily obtained using `__builtin_frame_address(0)`. Cosmo can
make this easier in certain cases. The `ShowCrashReports()` feature may
print the `cosmoaddr2line` command you'll need to run, to get a better
backtrace. On Windows, the Cosmopolitan runtime will output the command
to the `--strace` log whenever your program dies due to a fatal signal
that's blocked or in the `SIG_DFL` disposition.

### `mktemper`

The `mktemper` command is a portable replacement for the traditional
`mktemp` command, which isn't available on platforms like MacOS. Our
version also offers improvements, such as formatting a 64-bit random
value obtained from a cryptographic `getrandom()` entropy source. Using
this command requires passing an argument such as
`/tmp/foo.XXXXXXXXXXXXX` where the X's are replaced by a random value.
The newly created file is then printed to standard output.

## About

This toolchain is based on GCC. It's been modified too. We wrote a 2kLOC
patch which gives the C language the ability to `switch (errno) { case
EINVAL: ... }` in cases where constants like `EINVAL` are linkable
symbols. Your code will be rewritten in such cases to use a series of if
statements instead, so that Cosmopolitan Libc's system constants will
work as expected. Our modifications to GNU GCC are published under the
ISC license at <https://github.com/ahgamut/gcc/tree/portcosmo-11.2>. The
binaries you see here were first published at
<https://github.com/ahgamut/superconfigure/releases/tag/z0.0.30> which
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
