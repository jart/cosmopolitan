# Actually Portable Executable Specification v0.1

Actually Portable Executable (APE) is an executable file format that
polyglots the Windows Portable Executable (PE) format with a UNIX Sixth
Edition style shell script that doesn't have a shebang. This makes it
possible to produce a single file binary that executes on the stock
installations of the many OSes and architectures.

## Supported OSes and Architectures

- AMD64
  - Linux
  - MacOS
  - Windows
  - FreeBSD
  - OpenBSD
  - NetBSD
  - BIOS

- ARM64
  - Linux
  - MacOS
  - FreeBSD
  - Windows (non-native)

## File Header

APE defines three separate file magics, all of which are 8 characters
long. Any file that starts with one of these magic values can be
considered an APE program.

### (1) APE MZ Magic

- ASCII: `MZqFpD='`
- Hex: 4d 5a 71 46 70 44 3d 27

This is the canonical magic used by almost all APE programs. It enables
maximum portability between OSes. When interpreted as a shell script, it
is assigning a single quoted string to an unused variable. The shell
will then ignore subsequent binary content that's placed inside the
string.

It is strongly recommended that this magic value be immediately followed
by a newline (\n or hex 0a) character. Some shells, e.g. FreeBSD SH and
Zsh impose a binary safety check before handing off files that don't
have a shebang to `/bin/sh`. That check applies to the first line, which
can't contain NUL characters.

The letters were carefully chosen so as to be valid x86 instructions in
all operating modes. This makes it possible to store a BIOS bootloader
disk image inside an APE binary. For example, simple CLI programs built
with Cosmopolitan Libc will boot from BIOS into long mode if they're
treated as a floppy disk image.

The letters also allow for the possibility of being treated on x86-64 as
a flat executable, where the PE / ELF / Mach-O executable structures are
ignored, and execution simply begins at the beginning of the file,
similar to how MS-DOS .COM binaries work.

The 0x4a relative offset of the magic causes execution to jump into the
MS-DOS stub defined by Portable Executable. APE binaries built by Cosmo
Libc use tricks in the MS-DOS stub to check the operating mode and then
jump to the appropriate entrypoint, e.g. `_start()`.

#### Decoded as i8086

```asm
  dec  %bp
  pop  %dx
  jno  0x4a
  jo   0x4a
```

#### Decoded as i386

```asm
  push %ebp
  pop  %edx
  jno  0x4a
  jo   0x4a
```

#### Decoded as x86-64

```asm
  rex.WRB
  pop %r10
  jno 0x4a
  jo  0x4a
```

### (2) APE UNIX-Only Magic

- ASCII: `jartsr='`
- Hex: 6a 61 72 74 73 72 3d 27

Being a novel executable format that was first published in 2020, the
APE file format is less understood by industry tools compared to the PE,
ELF, and Mach-O executable file formats, which have been around for
decades. For this reason, APE programs that use the MZ magic above can
attract attention from Windows AV software, which may be unwanted by
developers who aren't interested in targeting the Windows platform.
Therefore the `jartsr='` magic is defined which enables the creation of
APE binaries that can safely target all non-Windows platforms. Even
though this magic is less common, APE interpreters and binfmt-misc
installations MUST support this.

It is strongly recommended that this magic value be immediately followed
by a newline (\n or hex 0a) character. Some shells, e.g. FreeBSD SH and
Zsh impose a binary safety check before handing off files that don't
have a shebang to `/bin/sh`. That check applies to the first line, which
can't contain NUL characters.

The letters were carefully chosen so as to be valid x86 instructions in
all operating modes. This makes it possible to store a BIOS bootloader
disk image inside an APE binary. For example, simple CLI programs built
with Cosmopolitan Libc will boot from BIOS into long mode if they're
treated as a floppy disk image.

The letters also allow for the possibility of being treated on x86-64 as
a flat executable, where the PE / ELF / Mach-O executable structures are
ignored, and execution simply begins at the beginning of the file,
similar to how MS-DOS .COM binaries work.

The 0x78 relative offset of the magic causes execution to jump into the
MS-DOS stub defined by Portable Executable. APE binaries built by Cosmo
Libc use tricks in the MS-DOS stub to check the operating mode and then
jump to the appropriate entrypoint, e.g. `_start()`.

#### Decoded as i8086 / i386 / x86-64

```asm
  push $0x61
  jb   0x78
  jae  0x78
```

### (3) APE Debug Magic

- ASCII: `APEDBG='`
- Hex: 41 50 45 44 42 47 3d 27

While APE files must be valid shell scripts, in practice, UNIX systems
will oftentimes be configured to provide a faster safer alternative to
loading an APE binary through `/bin/sh`. The Linux Kernel can be patched
to have execve() recognize the APE format and directly load its embedded
ELF header. Linux systems can also use binfmt-misc to recognize APE's MZ
and jartsr magic, and pass them to a userspace program named `ape` that
acts as an interpreter. In such environments, the need sometimes arises
to be able to test that the `/bin/sh` is working correctly, in which
case the `APEDBG='` magic is RECOMMENDED.

APE interpreters, execve() implementations, and binfmt-misc installs
MUST ignore this magic. If necessary, steps can be taken to help files
with this magic be passed to `/bin/sh` like a normal shebang-less shell
script for execution.

## Embedded ELF Header

APE binaries MAY embed an ELF header inside them. Unlike conventional
executable file formats, this header is not stored at a fixed offset.
It's instead encoded as octal escape codes in a shell script `printf`
statement. For example:

```
printf '\177ELF\2\1\1\011\0\0\0\0\0\0\0\0\2\0\076\0\1\0\0\0\166\105\100\000\000\000\000\000\060\013\000\000\000\000\000\000\000\000\000\000\000\000\000\000\165\312\1\1\100\0\070\0\005\000\0\0\000\000\000\000'
```

This `printf` statement MUST appear in the first 8192 bytes of the APE
executable, so as to limit how much of the initial portion of a file an
interpreter must load.

Multiple such `printf` statements MAY appear in the first 8192 bytes, in
order to specify multiple architectures. For example, fat binaries built
by the `apelink` program (provided by Cosmo Libc) will have two encoded
ELF headers, for AMD64 and ARM64, each of which point into the proper
file offsets for their respective native code. Therefore, kernels and
interpreters which load the APE format directly MUST check the
`e_machine` field of the `Elf64_Ehdr` that's decoded from the octal
codes, before accepting a `printf` shell statement as valid.

These printf statements MUST always use only unescaped ASCII characters
or octal escape codes. These printf statements MUST NOT use space saving
escape codes such as `\n`. For example, rather than saying `\n` it would
be valid to say `\012` instead. It's also valid to say `\12` but only if
the encoded characters that follow aren't an octal digit.

For example, the following algorithm may be used for parsing octal:

```c
static int ape_parse_octal(const unsigned char page[8192], int i, int *pc)
{
        int c;
        if ('0' <= page[i] && page[i] <= '7') {
                c = page[i++] - '0';
                if ('0' <= page[i] && page[i] <= '7') {
                        c *= 8;
                        c += page[i++] - '0';
                        if ('0' <= page[i] && page[i] <= '7') {
                                c *= 8;
                                c += page[i++] - '0';
                        }
                }
                *pc = c;
        }
        return i;
}
```

APE aware interpreters SHOULD only take `e_machine` into consideration.
It is the responsibility of the `_start()` function to detect the OS.
Therefore, multiple `printf` statements are only embedded in the shell
script for different CPU architectures.

The OS ABI field of an APE embedded `Elf64_Ehdr` SHOULD be set to
`ELFOSABI_FREEBSD`, since it's the only UNIX OS APE supports that
actually checks the field. However different values MAY be chosen for
binaries that don't intend to have FreeBSD in their support vector.

Counter-intuitively, the ARM64 ELF header is used on the MacOS ARM64
platform when loading from fat binaries.

## Embedded Mach-O Header (x86-64 only)

APE shell scripts that support MacOS on AMD64 must use the `dd` command
in a very specific way to specify how the embedded binary Macho-O header
is copied backward to the start of the file. For example:

```
dd if="$o" of="$o" bs=8 skip=433      count=66       conv=notrunc
```

These `dd` statements have traditionally been generated by the GNU as
and ld.bfd programs by encoding ASCII into 64-bit linker relocations,
which necessitated a fixed width for integer values. It took several
iterations over APE's history before we eventually got it right:

- `arg="  9293"` is how we originally had ape do it
- `arg=$((  9293))` b/c busybox sh disliked quoted space
- `arg=9293  ` is generated by modern apelink program

Software that parses the APE file format, which needs to extract the
Macho-O x86-64 header SHOULD support the old binaries that use the
previous encodings. To make backwards compatibility simple the following
regular expression may be used, which generalizes to all defined
formats:

```c
regcomp(&rx,
        "bs="              // dd block size arg
        "(['\"] *)?"       //   #1 optional quote w/ space
        "(\\$\\(\\( *)?"   //   #2 optional math w/ space
        "([[:digit:]]+)"   //   #3
        "( *\\)\\))?"      //   #4 optional math w/ space
        "( *['\"])?"       //   #5 optional quote w/ space
        " +"               //
        "skip="            // dd skip arg
        "(['\"] *)?"       //   #6 optional quote w/ space
        "(\\$\\(\\( *)?"   //   #7 optional math w/ space
        "([[:digit:]]+)"   //   #8
        "( *\\)\\))?"      //   #9 optional math w/ space
        "( *['\"])?"       //  #10 optional quote w/ space
        " +"               //
        "count="           // dd count arg
        "(['\"] *)?"       //  #11 optional quote w/ space
        "(\\$\\(\\( *)?"   //  #12 optional math w/ space
        "([[:digit:]]+)",  //  #13
        REG_EXTENDED);
```

For further details, see the canonical implementation in
`cosmopolitan/tool/build/assimilate.c`.

## Static Linking

Actually Portable Executables are always statically linked. This
revision of the specification does not define any facility for storing
code in dynamic shared objects.

Cosmopolitan Libc provides a solution that enables APE binaries have
limited access to dlopen(). By manually loading a platform-specific
executable and asking the OS-specific libc's dlopen() to load
OS-specific libraries, it becomes possible to use GPUs and GUIs. This
has worked great for AI projects like llamafile.

There is no way for an Actually Portable Executable to interact with
OS-specific dynamic shared object extension modules to programming
languages. For example, a Lua interpreter compiled as an Actually
Portable Executable would have no way of linking extension libraries
downloaded from the Lua Rocks package manager. This is primarily because
different OSes define incompatible ABIs.

While it was possible to polyglot PE+ELF+MachO to create multi-OS
executables, it simply isn't possible to do that same thing for
DLL+DYLIB+SO. Therefore, in order to have DSOs, APE would need to either
choose one of the existing formats or invent one of its own, and then
develop its own parallel ecosystem of extension software. In the future,
the APE specification may expand to encompass this. However the focus to
date has been exclusively on executables with limited dlopen() support.

## Application Binary Interface (ABI)

APE binaries use the System V ABI, as defined by:

- [System V ABI - AMD64 Architecture Processor Supplement](https://gitlab.com/x86-psABIs/x86-64-ABI)
- AARCH64 has a uniform consensus defined by ARM Limited

There are however a few changes we've had to make.

### No Red Zone

Actually Portable Executables that have Windows and/or bare metal in
their support vector MUST be compiled using `-mno-red-zone`. This is
because, on Windows, DLLs and other software lurking in the va-space
might use tricks like SetThreadContext() to take control of a thread
whereas on bare metal, it's also generally accepted that kernel-mode
code cannot assume a red zone either due to hardware interrupts that
pull the exact same kinds of stunts.

APE software that only has truly System V ABI conformant OSes (e.g.
Linux) in their support vector MAY use the red zone optimization.

### Thread Local Storage

#### aarch64

Here's the TLS memory layout on aarch64:

```
           x28
        %tpidr_el0
            │
            │    _Thread_local
        ┌───┼───┬──────────┬──────────┐
        │tib│dtv│  .tdata  │  .tbss   │
        ├───┴───┴──────────┴──────────┘
        │
    __get_tls()
```

The ARM64 code in actually portable executables use the `x28` register
to store the address of the thread information block. All aarch64 code
linked into these executables SHOULD be compiled with `-ffixed-x28`
which is supported by both Clang and GCC.

The runtime library for an actually portable executables MAY choose to
use `tpidr_el0` instead, if OSes like MacOS aren't being targeted. For
example, if the goal is to create a Linux-only fat binary linker program
for Musl Libc, then choosing to use the existing `tpidr_el0` convention
would be friction-free alternative.

It's not possible for an APE runtime that targets the full range of OSes
to use the `tpidr_el0` register for TLS because Apple won't allow it. On
MacOS ARM64 systems, this register can only be used by a runtime to
implement the `sched_getcpu()` system call. It's reserved by MacOS.

#### x86-64

Here's the TLS memory layout on x86_64:

```
                          __get_tls()
                              │
                             %fs OpenBSD/NetBSD
           _Thread_local      │
    ┌───┬──────────┬──────────┼───┐
    │pad│  .tdata  │  .tbss   │tib│
    └───┴──────────┴──────────┼───┘
                              │
   Linux/FreeBSD/Windows/Mac %gs
```

Quite possibly the greatest challenge in Actually Portable Executable
working, has been overcoming the incompatibilities between OSes in how
thread-local storage works on x86-64. The AMD64 architecture defines two
special segment registers. Every OS uses one of these segment registers
to implement TLS support. However not all OSes agree on which register
to use. Some OSes grant userspace the power to define either of these
registers to hold any value that is desired. Some OSes only effectively
allow a single one of them to be changed. Lastly, some OSes, e.g.
Windows, claim ownership of the memory layout these registers point
towards too.

Here's a breakdown on how much power is granted to userspace runtimes by
each OS when it comes to changing amd64 segment registers.

|         | %fs          | %gs          |
|---------|--------------|--------------|
| Linux   | unrestricted | unrestricted |
| MacOS   | inaccessible | unrestricted |
| Windows | inaccessible | restricted   |
| FreeBSD | unrestricted | unrestricted |
| NetBSD  | unrestricted | broken       |
| OpenBSD | unrestricted | inaccessible |

Therefore, regardless of which register one we choose, some OSes are
going to be incompatible.

APE binaries are always built with a Linux compiler. So another issue
arises in the fact that our Linux-flavored GCC and Clang toolchains
(which are used to produce cross-OS binaries) are also only capable of
producing TLS instructions that use the %fs convention.

To solve these challenges, the `cosmocc` compiler will rewrite binary
objects after they've been compiled by GCC, so that the `%gs` register
is used, rather than `%fs`. Morphing x86-64 binaries after they've been
compiled is normally difficult, due to the complexity of the machine
instruction language. However GCC provides `-mno-tls-direct-seg-refs`
which greatly reduces the complexity of this task. This flag forgoes
some optimizations to make the generated code simpler. Rather than doing
clever arithmetic with `%fs` prefixes, the compiler will always generate
the thread information block address load as a separate instruction.

```c
// Change AMD code to use %gs:0x30 instead of %fs:0
// We assume -mno-tls-direct-seg-refs has been used
static void ChangeTlsFsToGs(unsigned char *p, size_t n) {
  unsigned char *e = p + n - 9;
  while (p <= e) {
    // we're checking for the following expression:
    //   0144 == p[0] &&           // %fs
    //   0110 == (p[1] & 0373) &&  // rex.w (and ignore rex.r)
    //   (0213 == p[2] ||          // mov reg/mem → reg (word-sized)
    //   0003 == p[2]) &&          // add reg/mem → reg (word-sized)
    //   0004 == (p[3] & 0307) &&  // mod/rm (4,reg,0) means sib → reg
    //   0045 == p[4] &&           // sib (5,4,0) → (rbp,rsp,0) → disp32
    //   0000 == p[5] &&           // displacement (von Neumann endian)
    //   0000 == p[6] &&           // displacement
    //   0000 == p[7] &&           // displacement
    //   0000 == p[8]              // displacement
    uint64_t w = READ64LE(p) & READ64LE("\377\373\377\307\377\377\377\377");
    if ((w == READ64LE("\144\110\213\004\045\000\000\000") ||
         w == READ64LE("\144\110\003\004\045\000\000\000")) &&
        !p[8]) {
      p[0] = 0145;  // change %fs to %gs
      p[5] = 0x30;  // change 0 to 0x30
      p += 9;
    } else {
      ++p;
    }
  }
}
```

By favoring `%gs` we've now ensured friction-free compatibility for the
APE runtime on MacOS, Linux, and FreeBSD which are all able to conform
easily to this convention. However additional work needs to be done at
runtime when an APE program is started on Windows, OpenBSD, and NetBSD.
On these platforms, all executable pages must be faulted and morphed to
fixup the TLS instructions.

On OpenBSD and NetBSD, this is as simple as undoing the example
operation above. Earlier at compile-time we turned `%fs` into `%gs`.
Now, at runtime, `%gs` must be turned back into `%fs`. Since the
executable is morphing itself, this is easier said than done.

OpenBSD for example enforces a `W^X` invariant. Code that's executing
can't modify itself at the same time. The way Cosmopolitan solves this
is by defining a special part of the binary called `.text.privileged`.
This section is aligned to page boundaries. A GNU ld linker script is
used to ensure that code which morphs code is placed into this section,
through the use of a header-defined cosmo-specific keyword `privileged`.
Additionally, the `fixupobj` program is used by the Cosmo build system
to ensure that compiled objects don't contain privileged functions that
call non-privileged functions. Needless to say, `mprotect()` needs to be
a privileged function, so that it can be used to disable the execute bit
on all other parts of the executable except for the privileged section,
thereby making it writable. Once this has been done, code can change.

On Windows the displacement bytes of the TLS instruction are changed to
use the `%gs:0x1480+i*8` ABI where `i` is a number assigned by the WIN32
`TlsAlloc()` API. This avoids the need to call `TlsGetValue()` which is
implemented this exact same way under the hood. Even though 0x1480 isn't
explicitly documented by MSDN, this ABI is believed to be stable because
MSVC generates binaries that use this offset directly. The only caveat
is that `TlsAlloc()` must be called as early in the runtime init as
possible, to ensure an index less than 64 is returned.

### Thread Information Block (TIB)

The Actually Portable Executable Thread Information Block (TIB) is
defined by this version of the specification as follows:

- The 64-bit TIB self-pointer is stored at offset 0x00.
- The 64-bit TIB self-pointer is also stored at offset 0x30.
- The 32-bit `errno` value is stored at offset 0x3c.

All other parts of the thread information block should be considered
unspecified and therefore reserved for future specifications.

The APE thread information block is aligned on a 64-byte boundary.

Cosmopolitan Libc v3.5.8 (c. 2024-07-21) currently implements a thread
information block that's 512 bytes in size.

### Foreign Function Calls

Even though APE programs always use the System V ABI, there arises the
occasional need to interface with foreign functions, e.g. WIN32. The
`__attribute__((__ms_abi__))` annotation introduced by GCC v6 is used
for this purpose.

The ability to change a function's ABI on a case-by-case basis is
surprisingly enough supported by GCC, Clang, NVCC, and even the AMD HIP
compilers for both UNIX systems and Windows. All of these compilers
support both the System V ABI and the Microsoft x64 ABI.

APE binaries will actually favor the Microsoft ABI even when running on
UNIX OSes for certain dlopen() use-cases. For example, if we control the
code to a CUDA module, which we compile on each OS separately from our
main APE binary, then any function that's inside the APE binary whose
pointer may be passed into a foreign module SHOULD be compiled to use
the Microsoft ABI. This is because in practice the OS-specific module
may need to be compiled by MSVC, where MS ABI is the *only* ABI, which
forces our UNIX programs to partially conform. Thankfully, all UNIX
compilers support doing it on a case-by-case basis.

### Char Signedness

Actually Portable Executable defines `char` as signed.

Therefore conformant APE software MUST use `-fsigned-char` when building
code for aarch64, as well as any other architecture that (unlike x86-64)
would otherwise define `char` as being `unsigned char` by default.

This decision was one of the cases where it made sense to offer a more
consistent runtime experience for fat multi-arch binaries. However you
SHOULD still write code to assume `char` can go either way. But if all
you care about is using APE, then you CAN assume `char` is signed.

### Long Double

On AMD64 platforms, APE binaries define `long double` as 80-bit.

On ARM64 platforms, APE binaries define `long double` as 128-bit.

We accept inconsistency in this case, because hardware acceleration is
far more valuable than stylistic consistency in the case of mathematics.

One challenge arises on AMD64 for supporting `long double` across OSes.
Unlike UNIX systems, the Windows Executive on x86-64 initializes the x87
FPU to have double (64-bit) precision rather than 80-bit. That's because
code compiled by MSVC treats `long double` as though it were `double` to
prefer always using the more modern SSE instructions. However System V
requires genuine 80-bit `long double` support on AMD64.

Therefore, if an APE program detects that it's been started on a Windows
x86-64 system, then it SHOULD use the following assembly to initialize
the x87 FPU in System V ABI mode.

```asm
	fldcw	1f(%rip)
	.rodata
	.balign	2
//	8087 FPU Control Word
//	 IM: Invalid Operation ───────────────┐
//	 DM: Denormal Operand ───────────────┐│
//	 ZM: Zero Divide ───────────────────┐││
//	 OM: Overflow ─────────────────────┐│││
//	 UM: Underflow ───────────────────┐││││
//	 PM: Precision ──────────────────┐│││││
//	 PC: Precision Control ───────┐  ││││││
//	  {float,∅,double,long double}│  ││││││
//	 RC: Rounding Control ──────┐ │  ││││││
//	  {even, →-∞, →+∞, →0}      │┌┤  ││││││
//	                           ┌┤││  ││││││
//	                          d││││rr││││││
1:	.short	0b00000000000000000001101111111
	.previous
```

## Executable File Alignment

Actually Portable Executable is a statically-linked flat executable file
format that is, as a thing in itself, agnostic to file alignments. For
example, the shell script payload at the beginning of the file and its
statements have no such requirements. Alignment requirements are however
imposed by the executable formats that APE wraps.

1. ELF requires that file offsets be congruent with virtual addresses
   modulo the CPU page size. So when we add a shell script to the start
   of an executable, we need to round up to the page size in order to
   maintain ELF's invariant. Although no such roundup is required on the
   program segments once the invariant is restored. ELF loaders will
   happily map program headers from arbitrary file intervals (which may
   overlap) onto arbitrarily virtual intervals (which don't need to be
   contiguous). In order to do that, the loaders will generally use
   UNIX's mmap() function which is more restrictive and only accepts
   addresses and offsets that are page aligned. To make it possible to
   map an unaligned ELF program header that could potentially start and
   stop at any byte, ELF loaders round-out the intervals, which means
   adjacent unrelated data might also get mapped, which may need to be
   explicitly zero'd. Thanks to the cleverness of ELF, it's possible to
   have an executable file be very tiny, without needing any alignment
   bytes, and it'll be loaded into a properly aligned virtual space
   where segments can be as sparse as we want them to be.

2. PE doesn't care about congruence and instead defines two separate
   kinds of alignment. First, PE requires that the layout of segment
   memory inside the file be aligned on at minimum the classic 512 byte
   MS-DOS page size. This means that, unlike ELF, some alignment padding
   may need to be encoded into the file, making it slightly larger. Next
   PE imposes an alignment restriction on segments once they've been
   mapped into the virtual address space, which must be rounded to the
   system page size. Like ELF, PE segments need to be properly ordered
   but they're allowed to drift apart once mapped in a non-contiguous
   sparsely mapped way. When inserting shell script content at the start
   of a PE file, the most problematic thing is the need to round up to
   the 64kb system granularity, which results in a lot of needless bytes
   of padding being inserted by a naive second-pass linker.

3. Apple's Mach-O format is the strictest of them all. While both ELF
   and PE are defined in such a way that invites great creativity, XNU
   will simply refuse to an executable that does anything creative with
   alignment. All loaded segments need to both start and end on a page
   aligned address. XNU also wants segments to be contiguous similar to
   portable executable, except it applies to both the file and virtual
   spaces, which must follow the same structure.

Actually Portable Executables must conform to the strictest requirements
demanded by the support vector. Therefore an APE binary that has headers
for all three of the above executable formats MUST conform to the Apple
way of doing things. GNU ld linker scripts aren't very good at producing
ELF binaries that rigidly conform to this simple naive layout. There are
so many ways things can go wrong, where third party code might slip its
own custom section name in-between the linker script sections that are
explicitly defined, thereby causing ELF's powerful features to manifest
and the resulting content overlapping. The best `ld` flag that helps is
`--orphan-handling=error` which can help with explaining such mysteries.

While Cosmopolitan was originally defined to just use stock GNU tools,
this proved intractable over time, and the project has been evolving in
the direction of building its own. Inventing the `apelink` program was
what enabled the project to achieve multi-architecture binaries whereas
previously it was only possible to do multi-OS binaries. In the future,
our hope is that a fast power linker like Mold can be adapted to produce
fat APE binaries directly from object files in one pass.

## Position Independent Code

APE doesn't currently support position independent executable formats.
This is because APE was originally written for the GNU linker, where PIC
and PIE were after-thoughts and never fully incorporated with the older
more powerful linker script techniques upon which APE relies. Future
iterations of this specification are intended to converge on modern
standards, as our tooling becomes developed enough to support it.

However this only applies to the wrapped executable formats themselves.
While our convention to date has been to always load ELF programs at the
4mb mark, this is not guaranteed across OSes and architectures. Programs
should have no expectations that a program will be loaded to any given
address. For example, Cosmo currently implements APE on AARCH64 as
loading executables to a starting address of 0x000800000000. This
address occupies a sweet spot of requirements.

## Address Space

In order to create a single binary that supports as many platforms as
possible without needing to be recompiled, there's a very narrow range
of addresses that can be used. That range is somewhere between 32 bits
and 39 bits.

- Embedded devices that claim to be 64-bit will oftentimes only support
  a virtual address space that's 39 bits in size.

- We can't load executable images on AARCH64 beneath 0x100000000 (4gb)
  because Apple forbids doing that, possibly in an effort to enforce a
  best practice for spotting 32-bit to 64-bit transition bugs. Please
  note that this restriction only applies to Apple ARM64 systems. The
  x86-64 version of XNU will happily load APE binaries to 0x00400000.

- The AMD64 architecture on desktops and servers can usually be counted
  upon to provide a 47-bit address space. The Linux Kernel for instance
  grants each userspace program full dominion over addresses 0x00200000
  through 0x00007fffffffffff provided the hardware supports this. On
  modern workstations supporting Intel and AMD's new PML5T feature which
  virtualizes memory using a radix trie that's five layers deep, Linux
  is able to offer userspace its choice of fixed addresses from
  0x00200000 through 0x00ffffffffffffff. The only exception to this rule
  we've encountered so far is that Windows 7 and Windows Vista behaved
  similar to embedded devices in reducing the number of va bits.

## Page Size

APE software MUST be page size agnostic. For many years the industry had
converged on a strong consensus of having a page size that's 4096 bytes.
However this convention was never guaranteed. New computers have become
extremely popular, such as Apple Silicon, that use a 16kb page size.

By convention, Cosmopolitan Libc currently generates ELF headers for
x86-64 that are strictly aligned on a 4096-byte page size. On ARM64
Cosmopolitan is currently implemented to always generate ELF headers
aligned on a 16kb page size.

In addition to being page size agnostic, APE software that cares about
working correctly on Windows needs to be aware of the concept of
allocation granularity. While the page size on Windows is generally 4kb
in size, memory mappings can only be created on addresses that aligned
to the system allocation granularity, which is generally 64kb. If you
use a function like mmap() with Cosmopolitan Libc, then the `addr` and
`offset` parameters need to be aligned to `sysconf(_SC_GRANSIZE)` or
else your software won't work on Windows. Windows has other limitations
too, such as lacking the ability to carve or punch holes in mappings.
