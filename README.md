![Cosmopolitan Honeybadger](usr/share/img/honeybadger.png)

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

If you're doing your development work on Linux or BSD then you need just
five files to get started. Here's what you do on Linux:

```sh
wget https://justine.lol/cosmopolitan/cosmopolitan.zip
unzip cosmopolitan.zip
printf 'main() { printf("hello world\\n"); }\n' >hello.c
gcc -g -Os -static -nostdlib -nostdinc -fno-pie -no-pie -mno-red-zone \
  -fno-omit-frame-pointer -pg -mnop-mcount \
  -o hello.com.dbg hello.c -fuse-ld=bfd -Wl,-T,ape.lds \
  -include cosmopolitan.h crt.o ape-no-modify-self.o cosmopolitan.a
objcopy -S -O binary hello.com.dbg hello.com
```

You now have a portable program.

```sh
./hello.com
bash -c './hello.com'  # zsh/fish workaround (we patched them in 2021)
```

Since we used the `ape-no-modify-self.o` bootloader (rather than
`ape.o`) your executable will not modify itself when it's run. What
it'll instead do, is extract a 4kb program to `${TMPDIR:-/tmp}` that
maps your program into memory without needing to copy it. It's possible
to install the APE loader systemwide as follows.

```sh
# (1) linux systems that want binfmt_misc
ape/apeinstall.sh

# (2) for linux/freebsd/netbsd/openbsd systems
cp build/bootstrap/ape.elf /usr/bin/ape

# (3) for mac os x systems
cp build/bootstrap/ape.macho /usr/bin/ape
```

If you followed steps (2) and (3) then there's going to be a slight
constant-time startup latency each time you run an APE binary. Your
system might also prevent your APE program from being installed to a
system directory as a setuid binary or a script interpreter. To solve
that, you can use the following flag to turn your binary into the
platform local format (ELF or Mach-O):

```sh
./hello.com --assimilate
```

There's also some other useful flags that get baked into your binary by
default:

```sh
./hello.com --strace
./hello.com --ftrace
```

If you want your `hello.com` program to be much tinier, more on the
order of 16kb rather than 60kb, then all you have to do is use
<https://justine.lol/cosmopolitan/cosmopolitan-tiny.zip> instead. See
<https://justine.lol/cosmopolitan/download.html>.

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

## Source Builds

Cosmopolitan can be compiled from source on any Linux distro. GNU make
needs to be installed beforehand. This is a freestanding hermetic
repository that bootstraps using a vendored static gcc9 executable.
No further dependencies are required.

```sh
wget https://justine.lol/cosmopolitan/cosmopolitan.tar.gz
tar xf cosmopolitan.tar.gz  # see releases page
cd cosmopolitan
make -j16
o//examples/hello.com
find o -name \*.com | xargs ls -rShal | less
```

## GDB

Here's the recommended `~/.gdbinit` config:

```
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

```
gdb foo.com -ex 'add-symbol-file foo.com.dbg 0x401000'
```

## Support Vector

| Platform        | Min Version | Circa |
| :---            | ---:        | ---:  |
| AMD             | K8 Venus    | 2005  |
| Intel           | Core        | 2006  |
| New Technology  | Vista       | 2006  |
| GNU/Systemd     | 2.6.18      | 2007  |
| XNU's Not UNIX! | 15.6        | 2018  |
| FreeBSD         | 12          | 2018  |
| OpenBSD         | 6.4         | 2018  |
| NetBSD          | 9.1         | 2020  |
| GNU Make        | 4.0         | 2015  |
