![Cosmopolitan Honeybadger](usr/share/img/honeybadger.png)

# Cosmopolitan

[Cosmopolitan Libc](https://justine.lol/cosmopolitan/index.html) makes C
a build-once run-anywhere language, like Java, except it doesn't need an
interpreter or virtual machine. Instead, it reconfigures stock GCC to
output a POSIX-approved polyglot format that runs natively on Linux +
Mac + Windows + FreeBSD + OpenBSD + NetBSD + BIOS with the best possible
performance and the tiniest footprint imaginable.

## Background

For an introduction to this project, please read the [αcτµαlly pδrταblε
εxεcµταblε](https://justine.lol/ape.html) blog post and [cosmopolitan
libc](https://justine.lol/cosmopolitan/index.html) website. We also have
[API documentation](https://justine.lol/cosmopolitan/documentation.html).

## Getting Started

Cosmopolitan can be compiled from source on any Linux distro.

```sh
tar xf cosmopolitan-0.1.2.tar.gz  # see releases page
cd cosmopolitan-0.1.2
make -j16
o//examples/hello.com
find o -name \*.com | xargs ls -rShal | less
```

Alternatively you can use the release binaries:

```sh
unzip cosmopolitan-amalgamated-0.1.2.zip  # see releases page
echo 'main() { printf("hello world\n"); }' >hello.c
gcc -g -O -static -fno-pie -no-pie -mno-red-zone -nostdlib -nostdinc \
  -o hello.com.dbg hello.c -Wl,--gc-sections -Wl,-z,max-page-size=0x1000 -fuse-ld=bfd \
  -Wl,-T,ape.lds -include cosmopolitan.h crt.o ape.o cosmopolitan.a
objcopy -SO binary hello.com.dbg hello.com
./hello.com
```
