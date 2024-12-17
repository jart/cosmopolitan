# Cosmopolitan Toolchain

This toolchain can be used to compile executables that run on Linux /
MacOS / Windows / FreeBSD / OpenBSD 7.3 / NetBSD for both the x86_64 and
AARCH64 architectures. In addition to letting you create portable
binaries, your toolchain is itself comprised of portable binaries,
enabling you to have a consistent development environment that lets you
reach a broader audience from the platform(s) of your choosing.

## What's Included

This toolchain bundles GCC 14.1.0, Clang 19, Cosmopolitan Libc, LLVM
LIBCXX, LLVM compiler-rt, and LLVM OpenMP. Additional libraries were
provided by Musl Libc, and the venerable BSDs OSes. This lets you
benefit from the awesome modern GCC compiler with the strongest GPL
barrier possible. The preprocessor advertises cross compilers as both
`__COSMOCC__` and `__COSMOPOLITAN__` whereas `cosmocc` additionally
defines `__FATCOSMOCC__`.

## Getting Started

Once your toolchain has been extracted, you can compile hello world:

```
bin/cosmocc -o hello hello.c  # creates multi-os multi-arch binary
```

You now have an [actually portable
executable](https://justine.lol/ape.html) that'll run on your host
system. If anything goes wrong, see the Gotchas and Troubleshoot
sections below. It should have also outputted two ELF executables as
well, named `hello.dbg` (x86-64 Linux ELF) and `hello.aarch64.elf`
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


## Binary archive format

The APE format includes another portability superpower: the ability to 
distribute application support files WITHIN the compiled executable file. 
This is because APE files are also mostly regular zip files! You will 
need a copy of a compatible zip tool like the modified version of 
Info-ZIP available here: https://cosmo.zip/pub/cosmos/bin/zip. With this 
in hand the following command:

```sh
zip [APE file] [support_file.txt]
```

adds support_file.txt to your executable. You can see it listed within 
the archive with `unzip -l [APE file]`. 

Cosmo libc includes compatible file handling functions for accessing the 
contents of an APE file at the special '/zip' path. So your code is now 
able to do the following:

```c
if (access( "/zip/support_file.txt", F_OK) == 0) {
	fprintf(stderr, "/zip/support_file.txt FOUND and can be used as an asset\n");
}
```

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

## Flags

The following supplemental flags are defined by cosmocc:

- `-mcosmo` causes `_COSMO_SOURCE` to be defined. This has a similar
  effect to defining `_GNU_SOURCE`. When you use this flag, many
  non-standard GNU, BSD, and Cosmo Libc APIs will become visible in
  headers, e.g. `stdlib.h` will now define `ShowCrashReports()`.
  Including `cosmo.h` has a similar effect, however it's recommended
  that any program that uses cosmo-specific APIs pass this flag.

- `-mclang` (experimental) may be passed to the `cosmocc` command to use
  Clang instead of GCC under the hood. This can help C++ code compile 3x
  faster.

- `-mgcc` may be passed to the `cosmocc` command to use GCC instead of
  Clang under the hood. Since this is the default mode, this flag may be
  used to override the effect of passing the `-mclang` flag earlier.

- `-mdbg` may be passed when linking programs. It has the same effect as
  `export MODE=dbg` in that it will cause an alternative build of the
  Cosmopolitan Libc runtime to be linked that was built with `-O0 -g`.
  Under the normal build mode, `--ftrace` output generated by your libc
  is oftentimes missing important details due to inlining. If your build
  your code with `cosmocc -O0 -mdbg` then `--ftrace` will make much more
  sense. It's also the only way to make using GDB to troubleshoot issues
  inside Cosmo Libc work reliably. Please be warned, this flag enables
  some heavy-hitting runtime checks, such such lock graph validation.
  The debug Cosmopolitan runtime is able to detect lock cycles globally
  automatically via your normal usage of `pthread_mutex_t` and then
  report strongly connected components with C++ symbol demangling. This
  runtime will absolutely crash your entire process, if it helps you
  spot a bug. For example, debug cosmo is build with UBSAN so even an
  undiscovered yet innocent bit shift of a negative number could take
  you down. So you wouldn't want to use this in prod very often. Please
  note that passing `-mdbg` doesn't imply `-g -O0 -fsanitize=undefined`
  which must be passed separately if you want your code to be compiled
  with the same stuff as libc.

- `-mtiny` may be passed when linking programs. It has the same effect
  as `export MODE=tiny` in that it will cause an alternative build of
  the Cosmopolitan Libc runtime to be linked that's optimized for code
  size. In the normal build mode, the smallest possible binary size will
  be on the order of hundreds of kb, due to heavyweight features like
  `--ftrace` and `--strace` being part of the mandatory runtime. Those
  features don't exist in the tiny runtime, which should produce ~147kb
  fat binaries and ~36kb x86-only binaries. You may also use this flag
  when compiling objects. Since there's no function tracing, using this
  will eliminate the NOPs that get inserted into the prologues of your
  functions to make them hookable, which also greatly reduces code size.
  Please note that this does not specify an `-O` flag, so you may want
  to pass `-Os` too. Please note that this mode is granted leeway to
  trade away performance whenever possible. Functions like memmove()
  will stop using fancy vectorization which can dramatically decrease
  the performance of certain use cases. malloc() will no longer be
  scalable either. Cosmo malloc() will normally perform similarly to
  things like jemalloc. But in -mtiny mode it's protected by a GIL that
  may cause a multithreaded C++ HTTP server that makes intense usage of
  the STL may drop from 3.7 million requests per second to just 17k.
  We've seen it happen. malloc() will also stop using cookies which add
  bloat but are considered important by some people for both security
  and reporting errors on corruption. APIs will also begin refraining
  from detecting usage errors that are the fault of the caller, so this
  mode isn't recommended for development. Where -mtiny truly shines is
  when you're writing tiny programs. Particularly if they're ephemeral
  and frequent (e.g. build tooling), because the tiny runtime needs to
  do less work at process startup.

- `-moptlinux` uses the optimized Linux-only version of Cosmopolitan
  Libc runtime libraries. Your program will only be able to run on
  Linux. The runtime is compiled at `-O3` although it still supports AMD
  K8+ (c. 2003). Optimizations like red zone that wouldn't otherwise be
  possible are enabled. Function call tracing and system call logging is
  disabled. All the Windows polyfills go away and your binaries will be
  significantly tinier. The `cosmocc` compiler will generate a shell
  script with the magic `jartsr='` so you won't get unwanted attention
  from Windows virus scanners. You're even allowed to use flags like
  `-fomit-frame-pointer` when you use this mode. Users report optlinux
  has helped them make the Python interpreter 5% faster, like distros,
  optlinux will salt the earth if it gains a 1% advantage on benchmark
  games. Therefore this mode gives you an apples-to-apples comparison
  between cosmocc versus the gcc/clang configs used by linux distros.

## Raw Toolchains

The `cosmocc` and `cosmoar` programs use shell script magic to run both
toolchains under the hood. Sometimes this magic doesn't work when you're
building software that needs to do things like run the C preprocessor in
aarch64 mode. In such cases, cosmocc provides x86\_64 and aarch64 only
toolchains which give you more power and control over your builds.

- `x86_64-unknown-cosmo-cc`, `x86_64-unknown-cosmo-c++`, and
  `x86_64-linux-cosmo-as` let you build multi-OS programs that only run
  on x86\_64. You'll need this if you want to compile complex projects
  like Emacs and OpenSSL. These are shell scripts that help you make
  sure your software is compiled with the correct set of flags.

- `aarch64-unknown-cosmo-cc`, `aarch64-unknown-cosmo-c++`, and
  `aarch64-linux-cosmo-as` let you build multi-OS programs that only run
  on ARM64. You'll need this if you want to compile complex projects
  like Emacs and OpenSSL. These are shell scripts that help you make
  sure your software is compiled with the correct set of flags.

- `aarch64-linux-cosmo-cc`, `aarch64-linux-cosmo-c++`,
  `aarch64-linux-cosmo-as`, and `aarch64-linux-cosmo-ld` are the actual
  compiler executables. Using these grants full control over your
  compiler and maximum performance. This is the approach favored for
  instance by the Cosmopolitan Mono Repo's Makefile. If you use these,
  then you should have zero expectation of support, because you'll be
  assuming all responsibility for knowing about all the ABI-related
  flags your Cosmopolitan runtime requires.

When you use the "unknown" OS compilers, they'll link ELF executables
which embed an APE program image. This is so it's possible to have DWARF
debugging data. If you say:

```
x86_64-unknown-cosmo-cc -Os -mtiny -o hello hello.c
./hello
x86_64-linux-cosmo-objcopy -SO binary hello hello.com
./hello.com
```

Then you can unwap the raw stripped APE executable and get a much
smaller file than you otherwise would using the `-s` flag.

If you compile your software twice, using both the x86\_64 and aarch64
compilers, then it's possible to link the two binaries into a single fat
binary yourself via the `apelink` program. To understand how this
process works, it works best if you use the `BUILDLOG` variable, to see
how the shell script wrappers are doing it. You can also consult the
build configs of the ahgamut/superconfigure project on GitHub.

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
(cd /home/jart/cosmocc; bin/x86_64-linux-cosmo-gcc -o/tmp/fatcosmocc.ovdo2nqvkjjg3.dbg c...
(cd /home/jart/cosmocc; bin/aarch64-linux-cosmo-gcc -o/tmp/fatcosmocc.d3ca1smuot0k0.aarch64.elf /...
(cd /home/jart/cosmocc; bin/fixupobj /tmp/fatcosmocc.d3ca1smuot0k0.aarch64.elf)
(cd /home/jart/cosmocc; bin/fixupobj /tmp/fatcosmocc.ovdo2nqvkjjg3.dbg)
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
named `hello` then you'll need to pass either `hello.dbg` (x86-64)
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
ISC license at <https://github.com/ahgamut/gcc/tree/portcosmo-14.1>. The
binaries you see here were first published at
<https://github.com/ahgamut/superconfigure/releases/tag/z0.0.60> which
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
