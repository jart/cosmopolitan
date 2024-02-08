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

For an introduction to this project, please read the [actually portable
executable](https://justine.lol/ape.html) blog post and [cosmopolitan
libc](https://justine.lol/cosmopolitan/index.html) website. We also have
[API
documentation](https://justine.lol/cosmopolitan/documentation.html).

## Getting Started

You can start by obtaining a release of our `cosmocc` compiler from
<https://cosmo.zip/pub/cosmocc/>.

```sh
mkdir -p cosmocc
cd cosmocc
wget https://cosmo.zip/pub/cosmocc/cosmocc.zip
unzip cosmocc.zip
```

Here's an example program we can write:

```c
// hello.c
#include <stdio.h>

int main() {
  printf("hello world\n");
}
```

It can be compiled as follows:

```sh
cosmocc -o hello hello.c
./hello
```

The Cosmopolitan Libc runtime links some heavyweight troubleshooting
features by default, which are very useful for developers and admins.
Here's how you can log system calls:

```sh
./hello --strace
```

Here's how you can get a much more verbose log of function calls:

```sh
./hello --ftrace
```

You can use the Cosmopolitan's toolchain to build conventional open
source projects which use autotools. This strategy normally works:

```sh
export CC=x86_64-unknown-cosmo-cc
export CXX=x86_64-unknown-cosmo-c++
./configure --prefix=/opt/cosmos/x86_64
make -j
make install
```

## Cosmopolitan Source Builds

Cosmopolitan can be compiled from source on any of our supported
platforms. The Makefile will download cosmocc automatically.

It's recommended that you install a systemwide APE Loader. This command
requires `sudo` access to copy the `ape` command to a system folder and
register with binfmt_misc on Linux, for even more performance.

```sh
ape/apeinstall.sh
```

You can now build the mono repo with any modern version of GNU Make. To
make life easier, we've included one in the cosmocc toolchain, which is
guaranteed to be compatible and furthermore includes our extensions for
doing build system sandboxing.

```sh
build/bootstrap/make.com -j8
o//examples/hello.com
```

Since the Cosmopolitan repository is very large, you might only want to
build one particular thing. Here's an example of a target that can be
compiled relatively quickly, which is a simple POSIX test that only
depends on core LIBC packages.

```sh
rm -rf o//libc o//test
build/bootstrap/make.com o//test/posix/signal_test.com
o//test/posix/signal_test.com
```

Sometimes it's desirable to build a subset of targets, without having to
list out each individual one. For example if you wanted to build and run
all the unit tests in the `TEST_POSIX` package, you could say:

```sh
build/bootstrap/make.com o//test/posix
```

Cosmopolitan provides a variety of build modes. For example, if you want
really tiny binaries (as small as 12kb in size) then you'd say:

```sh
build/bootstrap/make.com m=tiny
```

You can furthermore cut out the bloat of other operating systems, and
have Cosmopolitan become much more similar to Musl Libc.

```sh
build/bootstrap/make.com m=tinylinux
```

For further details, see [//build/config.mk](build/config.mk).

## Debugging

To print a log of system calls to stderr:

```sh
cosmocc -o hello hello.c
./hello --strace
```

To print a log of function calls to stderr:

```sh
cosmocc -o hello hello.c
./hello --ftrace
```

Both strace and ftrace use the unbreakable kprintf() facility, which is
able to be sent to a file by setting an environment variable.

```sh
export KPRINTF_LOG=log
./hello --strace
```

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

## Platform Notes

### Shells

If you use zsh and have trouble running APE programs try `sh -c ./prog`
or simply upgrade to zsh 5.9+ (since we patched it two years ago). The
same is the case for Python `subprocess`, old versions of fish, etc.

### Linux

Some Linux systems are configured to launch MZ executables under WINE.
Other distros configure their stock installs so that APE programs will
print "run-detectors: unable to find an interpreter". For example:

```sh
jart@ubuntu:~$ wget https://cosmo.zip/pub/cosmos/bin/dash
jart@ubuntu:~$ chmod +x dash
jart@ubuntu:~$ ./dash
run-detectors: unable to find an interpreter for ./dash
```

You can fix that by registering APE with `binfmt_misc`:

```sh
sudo wget -O /usr/bin/ape https://cosmo.zip/pub/cosmos/bin/ape-$(uname -m).elf
sudo chmod +x /usr/bin/ape
sudo sh -c "echo ':APE:M::MZqFpD::/usr/bin/ape:' >/proc/sys/fs/binfmt_misc/register"
sudo sh -c "echo ':APE-jart:M::jartsr::/usr/bin/ape:' >/proc/sys/fs/binfmt_misc/register"
```

You should be good now. APE will not only work, it'll launch executables
400µs faster now too. However if things still didn't work out, it's also
possible to disable `binfmt_misc` as follows:

```sh
sudo sh -c 'echo -1 > /proc/sys/fs/binfmt_misc/cli'     # remove Ubuntu's MZ interpreter
sudo sh -c 'echo -1 > /proc/sys/fs/binfmt_misc/status'  # remove ALL binfmt_misc entries
```

### WSL

It's normally unsafe to use APE in a WSL environment, because it tries
to run MZ executables as WIN32 binaries within the WSL environment. In
order to make it safe to use Cosmopolitan software on WSL, run this:

```sh
sudo sh -c "echo -1 > /proc/sys/fs/binfmt_misc/WSLInterop"
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
