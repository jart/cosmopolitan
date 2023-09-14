![Cosmopolitan Honeybadger](usr/share/img/honeybadger.png)

[![build](https://github.com/jart/cosmopolitan/actions/workflows/build.yml/badge.svg)](https://github.com/jart/cosmopolitan/actions/workflows/build.yml)
# Cosmopolitan

[Cosmopolitan Libc](https://justine.lol/cosmopolitan/index.html) makes C
a build-once run-anywhere language, like Java, except it doesn't need an
interpreter or virtual machine. Instead, it reconfigures stock GCC and
Clang to output a POSIX-approved polyglot format that runs natively on
Linux + Mac + Windows + FreeBSD + OpenBSD + NetBSD + BIOS with the best
possible performance and the tiniest footprint imaginable.

## Background

For an introduction to this project, please read the [αcτµαlly pδrταblε
εxεcµταblε](https://justine.lol/ape.html) blog post and [cosmopolitan
libc](https://justine.lol/cosmopolitan/index.html) website. We also have
[API documentation](https://justine.lol/cosmopolitan/documentation.html).

## Getting Started

It's recommended that Cosmopolitan be installed to `/opt/cosmo` and
`/opt/cosmos` on your computer. The first has the monorepo. The second
contains your non-monorepo artifacts.

```sh
sudo mkdir -p /opt
sudo chmod 1777 /opt
git clone https://github.com/jart/cosmopolitan /opt/cosmo
export PATH="/opt/cosmo/bin:/opt/cosmos/bin:$PATH"
echo 'PATH="/opt/cosmo/bin:/opt/cosmos/bin:$PATH"' >>~/.profile
ape-install        # optionally install a faster systemwide ape loader
cosmocc --update   # pull cosmo and rebuild toolchain
```

You've now successfully installed your very own cosmos. Now let's build
an example program:

```c
// hello.c
#include <stdio.h>

int main() {
  printf("hello world\n");
}
```

To compile the program, you can run the `cosmocc` command. It's
important to give it an output path that ends with `.com` so the output
format will be Actually Portable Executable. When this happens, a
concomitant debug binary is created automatically too.

```sh
cosmocc -o hello.com hello.c
./hello.com
./hello.com.dbg
```

You can use the `cosmocc` toolchain to build conventional open source
projects which use autotools. This strategy normally works:

```sh
export CC=cosmocc
export CXX=cosmoc++
./configure --prefix=/opt/cosmos
make -j
make install
```

The Cosmopolitan Libc runtime links some heavyweight troubleshooting
features by default, which are very useful for developers and admins.
Here's how you can log system calls:

```sh
./hello.com --strace
```

Here's how you can get a much more verbose log of function calls:

```sh
./hello.com --ftrace
```

If you don't want rich runtime features like the above included, and you
just want libc, and you want smaller simpler programs. In that case, you
can consider using `MODE=tiny`, which is preconfigured by the repo in
[build/config.mk](build/config.mk). Using this mode is much more
effective at reducing binary footprint than the `-Os` flag alone. You
can change your build mode by doing the following:

```sh
export MODE=tiny
cosmocc --update
```

We can also make our program slightly smaller by using the system call
interface directly, which is fine, since Cosmopolitan polyfills these
interfaces across platforms, including Windows. For example:

```c
// hello2.c
#include <unistd.h>
int main() {
  write(1, "hello world\n", 12);
}
```

Once compiled, your APE binary should be ~36kb in size.

```sh
export MODE=tiny
cosmocc -Os -o hello2.com hello2.c
./hello2.com
```

But let's say you only care about your binaries running on Linux and you
don't want to use up all this additional space for platforms like WIN32.
In that case, you can try `MODE=tinylinux` for example which will create
executables more on the order of 8kb (similar to Musl Libc).

```sh
export MODE=tinylinux
cosmocc --update
cosmocc -Os -o hello2.com hello2.c
./hello2.com  # <-- actually an ELF executable
```

## ARM

Cosmo supports cross-compiling binaries for machines with ARM
microprocessors. For example:

```sh
make -j8 m=aarch64 o/aarch64/third_party/ggml/llama.com
make -j8 m=aarch64-tiny o/aarch64-tiny/third_party/ggml/llama.com
```

That'll produce ELF executables that run natively on two operating
systems: Linux Arm64 (e.g. Raspberry Pi) and MacOS Arm64 (i.e. Apple
Silicon), thus giving you full performance. The catch is you have to
compile these executables on an x86_64-linux machine. The second catch
is that MacOS needs a little bit of help understanding the ELF format.
To solve that, we provide a tiny APE loader you can use on M1 machines.

```sh
scp ape/ape-m1.c macintosh:
scp o/aarch64/third_party/ggml/llama.com macintosh:
ssh macintosh
xcode-install
cc -o ape ape-m1.c
sudo cp ape /usr/local/bin/ape
```

You can run your ELF AARCH64 executable on Apple Silicon as follows:

```sh
ape ./llama.com
```

If you want to run the `MODE=aarch64` unit tests, you need to have
qemu-aarch64 installed as a binfmt_misc interpreter. It needs to be a
static binary if you want it to work with Landlock Make's security. You
can use the build included in our `third_party/qemu/` folder.

```
doas cp o/third_party/qemu/qemu-aarch64 /usr/bin/qemu-aarch64
doas sh -c "echo ':qemu-aarch64:M::\x7fELF\x02\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\xb7\x00:\xff\xff\xff\xff\xff\xff\xff\x00\xff\xff\xff\xff\xff\xff\xff\xff\xfe\xff\xff\xff:/usr/bin/qemu-aarch64:CF' > /proc/sys/fs/binfmt_misc/register"
make -j8 m=aarch64
```

Please note that the qemu-aarch64 binfmt_misc interpreter installation
process is *essential* for being able to use the `aarch64-unknown-cosmo`
toolchain to build fat APE binaries on your x86-64 machine.

## AMD64 + ARM64 fat APE binaries

If you've setup the qemu binfmt_misc interpreter, then you can can use
cosmo's toolchains to build fat ape binaries. It works by compiling your
program twice, so you can have a native build for both architectures in
the same file. The two programs are merged together by apelink.com which
also embeds multiple copies of APE loader and multiple symbols tables.

The easiest way to build fat APE is using `fatcosmocc`. This compiler
works by creating a concomitant `.aarch64/foo.o` for every `foo.o` you
compile. The only exception is the C preprocessor mode, which actually
runs x86-64 GCC except with macros like `__x86_64__` undefined.

This toolchain works great for C projects that are written in a portable
way and don't produce architecturue-specific artifacts. One example of a
large project that can be easily built is GNU coreutils.

```sh
cd coreutils
fatcosmocc --update ||exit
./configure CC=fatcosmocc \
            AR=fatcosmoar \
            INSTALL=$(command -v fatcosmoinstall) \
            --prefix=/opt/cosmos \
            --disable-nls \
            --disable-dependency-tracking \
            --disable-silent-rules
make -j8
```

You'll then have a bunch of files like `src/ls` which are fat ape
binaries. If you want to run them on Windows, then you simply need to
rename the file so that it has the `.com` suffix. Better yet, consider
making that a symlink (a.k.a. reparse point). The biggest gotcha with
`fatcosmocc` though is ensuring builds don't strip binaries. For
example, Linux's `install -s` command actually understands Windows'
Portable Executable format well enough to remove the MS-DOS stub, which
is where the APE shell script is stored. You need to ensure that
`fatcosmoinstall` is used instead. Especially if your project needs to
install the libraries built by `fatacosmoar` into `/opt/cosmos`.

## Advanced Fat APE Builds

Once you get seriously involved in creating fat APE builds of software
you're going to eventually outgrow `fatcosmocc`. One example is Emacs
which is trickier to build, because it produces architecture-specific
files, and it also depends on shared files, e.g. zoneinfo. Since we like
having everything in a neat little single-file executable container that
doesn't need an "installation wizard", this tutorial will explain how we
manage to accomplish that.

What you're going to do is, instead of using `fatcosmocc`, you're going
to use both the `x86_64-unknown-cosmo-cc` and `aarch64-unknown-cosmo-cc`
toolchains independently, and then run `apelink` and `zip` to manually
build the final files. But there's a few tricks to learn first.

The first trick is to create a symlink on your system called `/zip`.
Cosmopolitan Libc normally uses that as a synthetic folder that lets you
access the assets in your zip executable. But since that's a read-only
file system, your build system should use the normal one.

```sh
doas ln -sf /opt/cosmos /zip
```

Now create a file named `rebuild-fat.sh` which runs the build twice:

```sh
#!/bin/sh
set -ex
export MODE=aarch64
export COSMOS=/opt/cosmos/aarch64
rebuild-cosmos.sh aarch64
export MODE=
export COSMOS=/opt/cosmos/x86_64
rebuild-cosmos.sh x86_64
wall.com 'finished building'
```

Then create a second file `rebuild-cosmos.sh` which runs your build:

```sh
#!/bin/bash
set -ex

ARCH=${1:-x86_64}
export COSMO=${COSMO:-/opt/cosmo}
export COSMOS=${COSMOS:-/opt/cosmos/$ARCH}
export AS=$(command -v $ARCH-unknown-cosmo-as) || exit
export CC=$(command -v $ARCH-unknown-cosmo-cc) || exit
export CXX=$(command -v $ARCH-unknown-cosmo-c++) || exit
export AR=$(command -v $ARCH-unknown-cosmo-ar) || exit
export STRIP=$(command -v $ARCH-unknown-cosmo-strip) || exit
export INSTALL=$(command -v $ARCH-unknown-cosmo-install) || exit
export OBJCOPY=$(command -v $ARCH-unknown-cosmo-objcopy) || exit
export OBJDUMP=$(command -v $ARCH-unknown-cosmo-objdump) || exit
export ADDR2LINE=$(command -v $ARCH-unknown-cosmo-addr2line) || exit

$CC --update

export COSMOPOLITAN_DISABLE_ZIPOS=1

cd ~/vendor/zlib
./configure --prefix=$COSMOS --static
make clean
make -j
make install

cd ~/vendor/ncurses-6.4
./configure --prefix=$COSMOS --sysconfdir=/zip --datarootdir=/zip/share --exec-prefix=/zip/$ARCH --disable-shared
make clean
make -j
make install

cd ~/vendor/readline-8.2
./configure --prefix=$COSMOS --sysconfdir=/zip --datarootdir=/zip/share --exec-prefix=/zip/$ARCH --disable-shared
make uninstall || true
make clean
make -j
make install

# NOTES:
# 1. You'll need to patch enum { FOO = x } that fails to build into a #define FOO
# 2. You'll need to patch configure.ac so it DOES NOT define USABLE_FIONREAD to 1
# 2. You'll need to patch configure.ac so it DOES NOT define INTERRUPT_INPUT to 1
cd ~/vendor/emacs-28.2
./configure --prefix=$COSMOS --sysconfdir=/zip --datarootdir=/zip/share --exec-prefix=/zip/$ARCH \
            --without-x --with-threads --without-gnutls --disable-silent-rules --with-file-notification=no
make uninstall || true
make clean
make -j
make install
```

Once you've completed this build process, you'll have the ELF files
`/opt/cosmos/x86_64/bin/emacs` and `/opt/cosmos/aarch64/bin/emacs`. Your
next move is to combine them into a single pristine `emacs.com` file.

```sh
cd /zip
COSMO=${COSMO:-/opt/cosmo}
mkdir -p /opt/cosmos/bin
apelink \
  -o /opt/cosmos/bin/emacs.com \
  -l "$COSMO/o//ape/ape.elf" \
  -l "$COSMO/o/aarch64/ape/ape.elf" \
  -M "$COSMO/ape/ape-m1.c" \
  /opt/cosmos/x86_64/bin/emacs \
  /opt/cosmos/aarch64/bin/emacs
cd /zip
zip -r /opt/cosmos/bin/emacs.com \
    aarch64/libexec \
    x86_64/libexec \
    share/terminfo \
    $(find share/emacs -type f |
        grep -v '\.el.gz$' |
        grep -v refcards |
        grep -v images)
```

You can now scp your `emacs.com` build to seven operating systems for
two distinct kinds of microprocessors without any dependencies. All the
LISP, zoneinfo, and termcap files it needs are stored inside the ZIP
structure of the binary, which has performance that's equivalent to the
Linux filesystem (even though it decompresses artifacts on the fly!) For
this reason, you might actually find that fat APE Emacs goes faster if
you're using an operating system like Windows where files are go slow.

If you like to use Vim instead of Emacs, then you can build that too.
However Vim's build system makes it a bit harder, since it's configured
to always strip binaries. The `apelink` program needs the symbol tables
to still be there when it creates the fat version. Otherwise tools like
`--ftrace` won't work.

## Monolithic Source Builds

Cosmopolitan can be compiled from source on any Linux distro. First, you
need to download or clone the repository.

```sh
wget https://justine.lol/cosmopolitan/cosmopolitan.tar.gz
tar xf cosmopolitan.tar.gz  # see releases page
cd cosmopolitan
```

This will build the entire repository and run all the tests:

```sh
build/bootstrap/make.com
o//examples/hello.com
find o -name \*.com | xargs ls -rShal | less
```

If you get an error running make.com then it's probably because you have
WINE installed to `binfmt_misc`. You can fix that by installing the the
APE loader as an interpreter. It'll improve build performance too!

```sh
bin/ape-install
```

Since the Cosmopolitan repository is very large, you might only want to
build a particular thing. Cosmopolitan's build config does a good job at
having minimal deterministic builds. For example, if you wanted to build
only hello.com then you could do that as follows:

```sh
build/bootstrap/make.com o//examples/hello.com
```

Sometimes it's desirable to build a subset of targets, without having to
list out each individual one. You can do that by asking make to build a
directory name. For example, if you wanted to build only the targets and
subtargets of the chibicc package including its tests, you would say:

```sh
build/bootstrap/make.com o//third_party/chibicc
o//third_party/chibicc/chibicc.com --help
```

Cosmopolitan provides a variety of build modes. For example, if you want
really tiny binaries (as small as 12kb in size) then you'd say:

```sh
build/bootstrap/make.com m=tiny
```

Here's some other build modes you can try:

```sh
build/bootstrap/make.com m=dbg       # asan + ubsan + debug
build/bootstrap/make.com m=asan      # production memory safety
build/bootstrap/make.com m=opt       # -march=native optimizations
build/bootstrap/make.com m=rel       # traditional release binaries
build/bootstrap/make.com m=optlinux  # optimal linux-only performance
build/bootstrap/make.com m=fastbuild # build 28% faster w/o debugging
build/bootstrap/make.com m=tinylinux # tiniest linux-only 4kb binaries
```

For further details, see [//build/config.mk](build/config.mk).

## Cosmopolitan Amalgamation

Another way to use Cosmopolitan is via our amalgamated release, where
we've combined everything into a single static archive and a single
header file. If you're doing your development work on Linux or BSD then
you need just five files to get started. Here's what you do on Linux:

```sh
wget https://justine.lol/cosmopolitan/cosmopolitan-amalgamation-2.2.zip
unzip cosmopolitan-amalgamation-2.2.zip
printf 'main() { printf("hello world\\n"); }\n' >hello.c
gcc -g -Os -static -nostdlib -nostdinc -fno-pie -no-pie -mno-red-zone \
  -fno-omit-frame-pointer -pg -mnop-mcount -mno-tls-direct-seg-refs -gdwarf-4 \
  -o hello.com.dbg hello.c -fuse-ld=bfd -Wl,-T,ape.lds -Wl,--gc-sections \
  -Wl,-z,common-page-size=0x1000 -Wl,-z,max-page-size=0x1000 \
  -include cosmopolitan.h crt.o ape-no-modify-self.o cosmopolitan.a
objcopy -S -O binary hello.com.dbg hello.com
```

You now have a portable program.

```sh
./hello.com
bash -c './hello.com'  # older zsh/fish workaround (patched in zsh 5.9 and fish 3.3.0)
```

If `./hello.com` executed on Linux throws an error about not finding an
interpreter, it should be fixed by running the following command (although
note that it may not survive a system restart):

```sh
sudo sh -c "echo ':APE:M::MZqFpD::/bin/sh:' >/proc/sys/fs/binfmt_misc/register"
```

If the same command produces puzzling errors on WSL or WINE when using
Redbean 2.x, they may be fixed by disabling binfmt_misc:

```sh
sudo sh -c 'echo -1 >/proc/sys/fs/binfmt_misc/status'
```

Since we used the `ape-no-modify-self.o` bootloader (rather than
`ape.o`) your executable will not modify itself when it's run. What
it'll instead do, is extract a 4kb program (the [APE loader](https://justine.lol/apeloader/))
to `${TMPDIR:-${HOME:-.}}` that maps your program into memory without
needing to copy it. The APE loader must be in an executable location
(e.g. not stored on a `noexec` mount) for it to run. See below for
alternatives:

It's possible to install the APE loader systemwide as follows.

```sh
# System-Wide APE Install
# for Linux, Darwin, and BSDs
# 1. Copies APE Loader to /usr/bin/ape
# 2. Registers w/ binfmt_misc too if Linux
bin/ape-install

# System-Wide APE Uninstall
# for Linux, Darwin, and BSDs
bin/ape-uninstall
```

It's also possible to convert APE binaries into the system-local format
by using the `--assimilate` flag. Please note that if binfmt_misc is in
play, you'll need to unregister it temporarily before doing this, since
the assimilate feature is part of the shell script header.

```sh
$ file hello.com
hello.com: DOS/MBR boot sector
./hello.com --assimilate
$ file hello.com
hello.com: ELF 64-bit LSB executable
```

Now that you're up and running with Cosmopolitan Libc and APE, here's
some of the most important troubleshooting tools APE offers that you
should know, in case you encounter any issues:

```sh
./hello.com --strace   # log system calls to stderr
./hello.com --ftrace   # log function calls to stderr
```

Do you love tiny binaries? If so, you may not be happy with Cosmo adding
heavyweight features like tracing to your binaries by default. In that
case, you may want to consider using our build system:

```sh
make m=tiny
```

Which will cause programs such as `hello.com` and `life.com` to shrink
from 60kb in size to about 16kb. There's also a prebuilt amalgamation
online <https://justine.lol/cosmopolitan/cosmopolitan-tiny.zip> hosted
on our download page <https://justine.lol/cosmopolitan/download.html>.

## GDB

Here's the recommended `~/.gdbinit` config:

```gdb
set host-charset UTF-8
set target-charset UTF-8
set target-wide-charset UTF-8
set osabi none
set complaints 0
set confirm off
set history save on
set history filename ~/.gdb_history
define asm
  layout asm
  layout reg
end
define src
  layout src
  layout reg
end
src
```

You normally run the `.com.dbg` file under gdb. If you need to debug the
`.com` file itself, then you can load the debug symbols independently as

```sh
gdb foo.com -ex 'add-symbol-file foo.com.dbg 0x401000'
```

## Alternative Development Environments

### MacOS

If you're developing on MacOS you can install the GNU compiler
collection for x86_64-elf via homebrew:

```sh
brew install x86_64-elf-gcc
```

Then in the above scripts just replace `gcc` and `objcopy` with
`x86_64-elf-gcc` and `x86_64-elf-objcopy` to compile your APE binary.

### Windows

If you're developing on Windows then you need to download an
x86_64-pc-linux-gnu toolchain beforehand. See the [Compiling on
Windows](https://justine.lol/cosmopolitan/windows-compiling.html)
tutorial. It's needed because the ELF object format is what makes
universal binaries possible.

Cosmopolitan officially only builds on Linux. However, one highly
experimental (and currently broken) thing you could try, is building the
entire cosmo repository from source using the cross9 toolchain.

```sh
mkdir -p o/third_party
rm -rf o/third_party/gcc
wget https://justine.lol/linux-compiler-on-windows/cross9.zip
unzip cross9.zip
mv cross9 o/third_party/gcc
build/bootstrap/make.com
```

## Discord Chatroom

The Cosmopolitan development team collaborates on the Redbean Discord
server. You're welcome to join us! <https://discord.gg/FwAVVu7eJ4>

## Support Vector

| Platform        | Min Version | Circa |
| :---            | ---:        | ---:  |
| AMD             | K8 Venus    | 2005  |
| Intel           | Core        | 2006  |
| Linux           | 2.6.18      | 2007  |
| Windows         | 8 [1]       | 2012  |
| Mac OS X        | 15.6        | 2018  |
| OpenBSD         | 7           | 2021  |
| FreeBSD         | 13          | 2020  |
| NetBSD          | 9.2         | 2021  |

[1] See our [vista branch](https://github.com/jart/cosmopolitan/tree/vista)
    for a community supported version of Cosmopolitan that works on Windows
    Vista and Windows 7.

## Special Thanks

Funding for this project is crowdsourced using
[GitHub Sponsors](https://github.com/sponsors/jart) and
[Patreon](https://www.patreon.com/jart). Your support is what makes this
project possible. Thank you! We'd also like to give special thanks to
the following groups and individuals:

- [Joe Drumgoole](https://github.com/jdrumgoole)
- [Rob Figueiredo](https://github.com/robfig)
- [Wasmer](https://wasmer.io/)

For publicly sponsoring our work at the highest tier.
