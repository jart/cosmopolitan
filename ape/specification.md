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
is assiging a single quoted string to an unused variable. The shell will
then ignore subsequent binary content that's placed inside the string.

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
intepreter must load.

Multiple such `printf` statements MAY appear in hte first 8192 bytes, in
order to specify multiple architectures. For example, fat binaries built
by the `apelink` program (provided by Cosmo Libc) will have two encoded
ELF headers, for amd64 and arm64, each of which point into the proper
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

Software that parses the APE file format, which needs to extract to be
able extract the Macho-O x86-64 header SHOULD support the old binaries
that use the previous encodings. To make backwards compatibility simple
the following regular expression may be used, which generalizes to all
defined formats:

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
