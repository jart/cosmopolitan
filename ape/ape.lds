/*-*- mode: ld-script; indent-tabs-mode: nil; tab-width: 2; coding: utf-8   -*-│
│vi: set et sts=2 tw=2 fenc=utf-8                                           :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╠──────────────────────────────────────────────────────────────────────────────╣
│░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░█▀█░█▀█░▀█▀░█░█░█▀█░█░░░█░░░█░█░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░█▀█░█░▄░░█░░█░█░█▀█░█░░░█░░░▀█▀░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░▀░▀░▀▀▀░░▀░░▀▀▀░▀░▀░▀▀▀░▀▀▀░░▀░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░█▀█░█▀█░█▀█░▀█▀░█▀█░█▀█░█░░░█▀▀░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░█▀▀░█ █░██▀░░█░░█▀█░█▀█░█░░░█▀▀░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░▀░░░▀▀▀░▀░▀░░▀░░▀░▀░▀▀▀░▀▀▀░▀▀▀░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░█▀▀░█░█░█▀▀░█▀█░█░█░▀█▀░█▀█░█▀█░█░░█▀▀░░░░░░░░░░░░░░░░░░░░░░░░▄▄░░░▐█░░│
│░░░░░░░█▀▀░▄▀▄░█▀▀░█░▄░█░█░░█░░█▀█░█▀█░█░░█▀▀░░░░░░░░░░░░▄▄▄░░░▄██▄░░█▀░░░█░▄░│
│░░░░░░░▀▀▀░▀░▀░▀▀▀░▀▀▀░▀▀▀░░▀░░▀░▀░▀▀▀░▀▀░▀▀▀░░░░░░░░░░▄██▀█▌░██▄▄░░▐█▀▄░▐█▀░░│
│░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░▐█▀▀▌░░░▄▀▌░▌░█░▌░░▌░▌░░│
╠──────────────────────────────────────────────────────▌▀▄─▐──▀▄─▐▄─▐▄▐▄─▐▄─▐▄─│
│ αcτµαlly pδrταblε εxεcµταblε § linker                                        │
╚──────────────────────────────────────────────────────────────────────────────╝
  Having an executable run natively on stock Windows / Mac / Linux / BSD
  entails two steps: (1) create a .com.dbg binary w/ Linux toolchain and
  then (2) unwrap the .com binary embedded within:

    objcopy -SO binary input.com.dbg output.com

  Both executables will work fine, but only the .com format is portable.

 ───BUILDING─────────────────────────────────────────────────────────────

    LC_ALL=C ld -T ape/ape.lds ...

 ───RUNNING──────────────────────────────────────────────────────────────

    ./foo.com.dbg                        # works on host machine
    ./foo.com                            # works on any os / arch
    qemu-system-x86_64 -s foo.com        # works on any os / arch

 ───BACKGROUND───────────────────────────────────────────────────────────

  The purpose of this software is to help native programs have the same
  level of consistency, in terms of user experience, that we enjoy with
  web applications. It's basically like MeteorJS, except primarily CLI,
  bootable, and more on the order of a few kilobytes than hundred megs.

  Rather than Isomorphic JavaScript it's built using Isomorphic Binary,
  since it grants the fastest possible performance and can be trivially
  emulated in the browser. System resource utilization is also a few kb
  and GUIs are possible too since Cosmopolitan exports the Windows API,
  but we recommend doing it with a CLI web server instead and embedding
  files in your αcτµαlly pδrταblε εxεcµταblε as it's isomorphic to zip.

  Isomorphic Binary principles state that most platform differences are
  just numbers, which we integrate easily into a unified business logic
  through the use of a sufficiently powerful linker. System numbers are
  otherwise known as ABIs and they're usually the most stable canonical
  interfaces that platforms provide. This is how we are able to support
  more versions of Linux than most Linux-only software, e.g. glibc FTMP

 ───DEBUGGING────────────────────────────────────────────────────────────

  Can be done in a few ways:

    gdb --tui foo.com.dbg
    gdb --tui foo.com -ex 'add-symbol-file foo.com.dbg 0x200000'
    gdb --tui -ex 'add-symbol-file foo.com.dbg 0x7c00'   \
              -ex 'add-symbol-file foo.com.dbg 0x200000' \
              -ex -target remote localhost:1234'

 ───TRANSPARENCY─────────────────────────────────────────────────────────

  αcτµαlly pδrταblε εxεcµταblε is designed to facilitate maximum
  transparency to engender trust in this linker process.

  The headers and symbols can be viewed using readelf or objdump:

    readelf -Wa input.com.dbg               # maximum transparency
    objdump -wxd input.com.dbg              # maximum transparency

  The disassembly can be viewed using objdump:

    readelf -Wa input.com.dbg               # maximum transparency
    objdump -d input.com.dbg                # maximum transparency
    objdump -dj.text input.com.dbg          # skip αpε boilerplate
    objdump -j.load -dMi8086 input.com.dbg  # fixes real mode code

  Some commands for controlling the verbosity of binaries:

    strip -X input.com.dbg                  # remove ".L" symbols
    strip input.com.dbg                     # remove all symbols
    strip -S input.com.dbg                  # remove debug info only
    make CPPFLAGS=-DNDEBUG                  # remove asserts (prod)
    make CPPFLAGS=-DIM_FEELING_NAUGHTY      # remove legal embeddings

  The Makefile build is also configured to always produce a .map file
  when building each program, which provides further details.

 ───HACKABILITY──────────────────────────────────────────────────────────

  Your linker and assemblies were designed provide extensibility through
  the use of link-time data structures we call "decentralized sections".
  They allow functions like _init() to be comprised of many small pieces
  defined throughout the codebase. The same applies to ELF / PE headers.

  Extending that content usually entails writing a .S file. The process
  has more in common with JavaScript programming than contemporary C++
  development practices. It's the reason Cosmopolitan is able to build
  the fast tiny multiplatform autonomous binaries that indie developers
  love using a scalable development model that big businesses respect.

 ───SECURITY─────────────────────────────────────────────────────────────

  αcτµαlly pδrταblε εxεcµταblε is designed to be secure in untrustworthy
  computing environments. Code and data are separated. Data structures
  initialized at startup are automatically memory protected afterwards.
  Code intended for platforms you don't use is automatically unmapped
  too, minimizing any possible chance of impacting your system, while
  still being there in case you ever need it.

 ───CONFIDENTIALITY──────────────────────────────────────────────────────

  αcτµαlly pδrταblε εxεcµταblε is also designed to not leak confidential
  information by default. Details relating to the host build environment
  such as system/library versions, user ids, home folder locations, etc.
  are not taken into consideration at build time since it's hermetic. We
  can't make speak for debug information, which is why it's put in other
  files. We also provide the bing and fold programs for auditing binary.

 ───DESIGN─DETAILS───────────────────────────────────────────────────────

  αcτµαlly pδrταblε εxεcµταblε is a non-reflective (a.k.a. flat) binary
  format that includes ELF, PE, and Macho-O headers only to respect the
  initialization rituals that supported platforms require.

  Binaries are sparse because Intel's six thousand page manual says:

    “Always put code and data on separate pages. [...] If code is
     to be modified,  try to do it all at once and  make sure the
     code that  performs the  modifications and  the  code  being
     modified are on separate 4KByte pages or on separate aligned
     1-KByte subpages. [...]   If (hopefully read-only) data must
     occur on the same page as code, avoid placing it immediately
     after an indirect jump [...]  or inserting an illegal opcode
     [...]  after the indirect branch [which] may degrade perf in
     some circumstances.” ──Intel V.O §3.6.9

  Support for linking dynamic shared objects is only implemented on
  Windows NT for the reasons described by Ulrich Drepper in his DSO
  tutorial. We've implemented this independently of the ld codebase
  because authentic GNU tooling is powerful enough to generalize to
  arbitrary formats without needing to add features to its codebase.

  Cosmopolitan core library functions may be converted to the COFF or
  Mach-O object formats using objconv. That gives you some freedom to
  choose to use the Microsoft or Apple linker instead of this one. We
  otherwise can't use those formats, due to how they heavily restrict
  naming, which basically makes everything we're doing impossible. In
  the future an authentic GNU toolchain will be made available on the
  Windows and Apple platforms, using canonical formats and behaviors.
  Until then, we can build for those platforms using Linux or WSL. */

#ifdef __LINKER__
#include "ape/macros.internal.h"
#include "ape/config.h"
#include "libc/nt/pedef.internal.h"
#include "libc/zip.h"

ENTRY(_start)

/* Plans real memory solution at linktime. */
MEMORY {
  PageZero :    org = 0x0000000000000000,  len = 0x0000000000001000
  RealBss :     org = XLM_BASE_REAL,       len = XLM_SIZE
  RealProgram : org = IMAGE_BASE_REAL,     len = 0x0000000000010000 - IMAGE_BASE_REAL
  RealScratch : org = REAL_SCRATCH_AREA,   len = REAL_STACK_FRAME - REAL_SCRATCH_AREA
  RealStack :   org = REAL_STACK_FRAME,    len = 0x0000000000010000
  EbdaMemory :  org = 0x0000000000080000,  len = 0x0000000000020000
  VideoMemory : org = 0x00000000000a0000,  len = 0x0000000000020000
  Romz :        org = 0x00000000000c0000,  len = 0x0000000000030000
  BiosMemory :  org = 0x00000000000f0000,  len = 0x0000000000010000
  SmallCode :   org = IMAGE_BASE_PHYSICAL, len = 0x0000000040000000 - IMAGE_BASE_PHYSICAL
  ZipData :     org = 0x0000000040000000,  len = 0x0000000040000000
}

PHDRS {
  Head PT_LOAD FLAGS(5);
  Rom PT_LOAD FLAGS(5);
  Ram PT_LOAD FLAGS(6);
  stack PT_GNU_STACK FLAGS(6);
}

SECTIONS {

/*BEGIN: realmode addressability guarantee */
/*BEGIN: xnu addressability guarantee */
/*BEGIN: linux addressability guarantee */
/*BEGIN: bsd addressability guarantee */

  .head SEGMENT_START("text-segment", IMAGE_BASE_VIRTUAL) : {
    HIDDEN(_base = .);

    /* Real Mode */
    KEEP(*(.head))
    . += 1;

    /* Executable & Linkable Format */
    . = ALIGN(. != 0 ? __SIZEOF_POINTER__ : 1);
    KEEP(*(.elf.phdrs))
    HIDDEN(.Lape.phdrs.end = .);
    . += 1;

    /* OpenBSD */
    . = ALIGN(. != 0 ? __SIZEOF_POINTER__ : 1);
    HIDDEN(.Lape.note = .);
    KEEP(*(.note.openbsd.ident))
    HIDDEN(.Lape.note.end = .);
    . += 1;

    /* Portable Executable */
    KEEP(*(.pe.header))
    HIDDEN(.Lape.pe.sections = .);
    KEEP(*(.pe.sections))
    HIDDEN(.Lape.pe.sections_end = .);
    . += 1;

    /* Mach-O */
    KEEP(*(.macho))
    . = ALIGN(__SIZEOF_POINTER__);
    HIDDEN(.Lape.macho.end = .);
    . += 1;

    KEEP(*(.ape.pad.head))
    . = ALIGN(4096); /* alignments only mandatory when impossible otherwise */
    HIDDEN(_ehead = .);
  } AT>SmallCode :Head

/*BEGIN: nt addressability guarantee */

  .text . : {
    /* Code that needs to be addressable in Real Mode */
    *(.text.real)
    KEEP(*(SORT_BY_NAME(.sort.text.real.*)))
    *(.rodata.real)
    KEEP(*(SORT_BY_NAME(.sort.rodata.real.*)))
    HIDDEN(_ereal = .);

/*END: realmode addressability guarantee */

    /* Normal Code */
    *(.start)
    KEEP(*(.initprologue))
    KEEP(*(SORT_BY_NAME(.init.*)))
    KEEP(*(SORT_NONE(.init)))
    KEEP(*(.initepilogue))
    KEEP(*(.pltprologue))
    *(.plt)
    KEEP(*(.pltepilogue))
    KEEP(*(.pltgotprologue))
    *(.plt.got)
    KEEP(*(.pltgotepilogue))
    *(.text.startup .text.startup.*)
    *(.text.exit .text.exit.*)
    *(.text.unlikely .text.*_unlikely .text.unlikely.*)
    *(SORT_BY_ALIGNMENT(.text.antiquity))
    *(SORT_BY_ALIGNMENT(.text.antiquity.*))
    KEEP(*(.textwindowsprologue))
    *(.text.windows)
    KEEP(*(.textwindowsepilogue))
    *(SORT_BY_ALIGNMENT(.text.modernity))
    *(SORT_BY_ALIGNMENT(.text.modernity.*))
    *(SORT_BY_ALIGNMENT(.text.hot))
    *(SORT_BY_ALIGNMENT(.text.hot.*))
    KEEP(*(.keep.text))
    *(.text .stub .text.*)
    KEEP(*(SORT_BY_NAME(.sort.text.*)))

    /* Won't support NX bit DRM for tiny executables */
    HIDDEN(.Lape.piro.align = ABSOLUTE(. > APE_PIRO_THRESHOLD ? 0x1000 : 8));

    /* Code that musn't be mapped in production */
    KEEP(*(.ape.pad.test));
    . = ALIGN(.Lape.piro.align);
    HIDDEN(__test_start = .);
    *(.test.unlikely)
    *(.test .test.*)

    /* Privileged code invulnerable to magic */
    KEEP(*(.ape.pad.privileged));
    . = ALIGN(.Lape.piro.align);
    HIDDEN(__privileged_start = .);
    HIDDEN(__test_end = .);
    . += 1;
    *(.privileged)

/*BEGIN: Read Only Data */

    KEEP(*(.ape.pad.rodata));
    . = ALIGN(.Lape.piro.align);
    . += 1;

    /* Nonspecific Read-Only Data */
    *(.rodata .rodata.*)
    . += 1;

    /* Undefined Behavior Sanitizer Types */
    HIDDEN(__ubsan_types_start = .);
    *(.ubsan.types)
    HIDDEN(__ubsan_types_end = .);
    . += 1;

    /* Undefined Behavior Sanitizer Data */
    HIDDEN(__ubsan_data_start = .);
    *(.ubsan.data)
    HIDDEN(__ubsan_data_end = .);

    /* Unit Test & Fixture Registry */

/*BEGIN: Read only data that needn't be mapped after initialization */

    /* Legal Notices */
#if !defined(IM_FEELING_NAUGHTY) || defined(EMBED_NOTICES)
    KEEP(*(.commentprologue))
    KEEP(*(.comment))
    KEEP(*(.commentepilogue))
#endif

    /* Windows DLL Import Directory */
    KEEP(*(.idata.ro));
    KEEP(*(SORT_BY_NAME(.idata.ro.*)))
    . += 1;

    /* Encoded Data Structures w/ Linear Initialization Order */
    KEEP(*(.initroprologue))
    KEEP(*(SORT_BY_NAME(.initro.*)))
    KEEP(*(.initroepilogue))
    KEEP(*(SORT_BY_NAME(.sort.rodata.*)))

    KEEP(*(.ape.pad.text))
    HIDDEN(.Lape.data.align = ABSOLUTE(PAGESIZE));
    . = ALIGN(.Lape.data.align);
    HIDDEN(_etext = .);
    PROVIDE_HIDDEN(etext = .);
/*END: Read Only Data (only needed for initialization) */
/*END: Read Only Data */
  } AT>SmallCode :Rom

  .data . : {
/*BEGIN: Read/Write Data */
    KEEP(*(.dataprologue))
    *(.data .data.*)
    KEEP(*(SORT_BY_NAME(.sort.data.*)))
    . += . > 0 ? 1 : 0;

    KEEP(*(.gotprologue))
    *(.got)
    KEEP(*(.gotepilogue))

    KEEP(*(.gotpltprologue))
    *(.got.plt)
    KEEP(*(.gotpltepilogue))

/*BEGIN: Post-Initialization Read-Only */
    . = ALIGN(.Lape.piro.align);
    HIDDEN(__piro_start = .);

    QUAD(IMAGE_BASE_VIRTUAL);

    PROVIDE_HIDDEN(__init_array_start = .);
    KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*)
           SORT_BY_INIT_PRIORITY(.ctors.*)))
    KEEP(*(SORT_NONE(.ctors)))
    KEEP(*(SORT_NONE(.init_array)))
    KEEP(*(SORT_NONE(.preinit_array)))
    PROVIDE_HIDDEN(__init_array_end = .);

    . += 1;
    . = ALIGN(__SIZEOF_POINTER__);
    PROVIDE_HIDDEN(__fini_array_start = .);
    KEEP(*(SORT_BY_INIT_PRIORITY(.fini_array.*)
           SORT_BY_INIT_PRIORITY(.dtors.*)))
    KEEP(*(SORT_NONE(.dtors)))
    PROVIDE_HIDDEN(__fini_array_end = .);

    KEEP(*(SORT_BY_NAME(.piro.relo.sort.*)))
    PROVIDE_HIDDEN(__relo_end = .);
    KEEP(*(SORT_BY_NAME(.piro.data.sort.*)))
    KEEP(*(.piro.pad.data))
    . = ALIGN(.Lape.data.align);
    HIDDEN(_edata = .);
    PROVIDE_HIDDEN(edata = .);
  } AT>SmallCode :Ram

  .bss . : {
    KEEP(*(SORT_BY_NAME(.piro.bss.init.*)))
    *(.piro.bss)
    KEEP(*(SORT_BY_NAME(.piro.bss.sort.*)))
    . += 1;
    . = ALIGN(.Lape.piro.align);
    HIDDEN(__piro_end = .);
/*END: Post-Initialization Read-Only */

    /* Statically Allocated Empty Space */
    *(SORT_BY_ALIGNMENT(.bss))
    *(SORT_BY_ALIGNMENT(.bss.*))
    *(COMMON)

    KEEP(*(SORT_BY_NAME(.sort.bss.*)))

    /* eXtreme Low Memory w/ Userspace Remapping */
    . = ALIGN(0x1000);
    *(.xlm)
    . = ALIGN(0x1000);

    HIDDEN(_end = .);
    PROVIDE_HIDDEN(end = .);
  } AT>SmallCode :Ram

/*END: nt addressability guarantee */
/*END: bsd addressability guarantee */
/*END: linux addressability guarantee */
/*END: xnu addressability guarantee */

  .stab              0 : { *(.stab) }
  .stabstr           0 : { *(.stabstr) }
  .stab.excl         0 : { *(.stab.excl) }
  .stab.exclstr      0 : { *(.stab.exclstr) }
  .stab.index        0 : { *(.stab.index) }
  .stab.indexstr     0 : { *(.stab.indexstr) }
  .comment           0 : { *(.comment) }
  .debug             0 : { *(.debug) }
  .line              0 : { *(.line) }
  .debug_srcinfo     0 : { *(.debug_srcinfo) }
  .debug_sfnames     0 : { *(.debug_sfnames) }
  .debug_aranges     0 : { *(.debug_aranges) }
  .debug_pubnames    0 : { *(.debug_pubnames) }
  .debug_info        0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev      0 : { *(.debug_abbrev) }
  .debug_line        0 : { *(.debug_line .debug_line.* .debug_line_end ) }
  .debug_frame       0 : { *(.debug_frame) }
  .debug_str         0 : { *(.debug_str) }
  .debug_loc         0 : { *(.debug_loc) }
  .debug_macinfo     0 : { *(.debug_macinfo) }
  .debug_weaknames   0 : { *(.debug_weaknames) }
  .debug_funcnames   0 : { *(.debug_funcnames) }
  .debug_typenames   0 : { *(.debug_typenames) }
  .debug_varnames    0 : { *(.debug_varnames) }
  .debug_pubtypes    0 : { *(.debug_pubtypes) }
  .debug_ranges      0 : { *(.debug_ranges) }
  .debug_macro       0 : { *(.debug_macro) }
  .debug_addr        0 : { *(.debug_addr) }
  .gnu.attributes    0 : { KEEP(*(.gnu.attributes)) }
  .GCC.command.line  0 : { *(.GCC.command.line) }

  /DISCARD/ : {
    *(.discard)
    *(.yoink)
    *(.*)
  }
}

PFSTUB8(.Lape.elf.entry, _start);
PFSTUB8(.Lape.elf.phoff, RVA(ape.phdrs));
PFSTUB8(.Lape.elf.shoff, 0);
PFSTUB4(.Lape.elf.phnum, (.Lape.phdrs.end - ape.phdrs) / 56);
PFSTUB4(.Lape.elf.shnum, 0);
PFSTUB4(.Lape.elf.shstrndx, 0);
SHSTUB2(.Lape.macho.dd.skip, RVA(ape.macho) / 8);
SHSTUB2(.Lape.macho.dd.count, (.Lape.macho.end - ape.macho) / 8);
PFSTUB4(.Lape.pe.offset, ape.pe - ape.mz);

HIDDEN(.Lape.pe.optsz = .Lape.pe.sections - (ape.pe + 24));
HIDDEN(.Lape.pe.shnum = (.Lape.pe.sections_end - .Lape.pe.sections) / 40);
HIDDEN(.Lidata.idtsize = idata.idtend - idata.idt);
HIDDEN(.Lidata.iatsize = idata.iatend - idata.iat);

HIDDEN(.Lape.rom.offset = 0);
HIDDEN(.Lape.rom.vaddr = ADDR(.head));
HIDDEN(.Lape.rom.paddr = LOADADDR(.head));
HIDDEN(.Lape.rom.filesz = LOADADDR(.data) - .Lape.rom.paddr);
HIDDEN(.Lape.rom.memsz = ADDR(.data) - ADDR(.head));
HIDDEN(.Lape.rom.align = 0x1000);
HIDDEN(.Lape.rom.rva = RVA(.Lape.rom.vaddr));

HIDDEN(.Lape.ram.offset = .Lape.rom.offset + .Lape.rom.filesz);
HIDDEN(.Lape.ram.vaddr = ADDR(.data));
HIDDEN(.Lape.ram.paddr = LOADADDR(.data));
HIDDEN(.Lape.ram.filesz = LOADADDR(.bss) - LOADADDR(.data));
HIDDEN(.Lape.ram.memsz = ADDR(.bss) + SIZEOF(.bss) - .Lape.ram.vaddr);
HIDDEN(.Lape.ram.align = 0x1000);
HIDDEN(.Lape.ram.rva = RVA(.Lape.ram.vaddr));

HIDDEN(.Lape.note.offset = .Lape.rom.offset + (.Lape.note - .Lape.rom.vaddr));
HIDDEN(.Lape.note.vaddr = .Lape.note);
HIDDEN(.Lape.note.paddr = .Lape.rom.paddr + .Lape.note.offset);
HIDDEN(.Lape.note.filesz = .Lape.note.end - .Lape.note);
HIDDEN(.Lape.note.memsz = .Lape.note.filesz);
HIDDEN(.Lape.note.align = __SIZEOF_POINTER__);

HIDDEN(.Lape.text.offset = .Lape.rom.offset + LOADADDR(.text) - .Lape.rom.paddr);
HIDDEN(.Lape.text.paddr = LOADADDR(.text));
HIDDEN(.Lape.text.vaddr = ADDR(.text));
HIDDEN(.Lape.text.filesz = SIZEOF(.text));
HIDDEN(.Lape.text.memsz = SIZEOF(.text));
HIDDEN(.Lape.text.align = 4096);
HIDDEN(.Lape.text.rva = RVA(.Lape.text.vaddr));

HIDDEN(.Lape.data.offset = .Lape.ram.offset + LOADADDR(.data) - .Lape.ram.paddr);
HIDDEN(.Lape.data.paddr = LOADADDR(.data));
HIDDEN(.Lape.data.vaddr = ADDR(.data));
HIDDEN(.Lape.data.filesz = SIZEOF(.data));
HIDDEN(.Lape.data.memsz = SIZEOF(.data));
HIDDEN(.Lape.data.align = 0x1000);
HIDDEN(.Lape.data.rva = RVA(.Lape.data.vaddr));

HIDDEN(.Lape.bss.offset = .Lape.ram.offset + LOADADDR(.bss) - .Lape.ram.paddr);
HIDDEN(.Lape.bss.paddr = LOADADDR(.bss));
HIDDEN(.Lape.bss.vaddr = ADDR(.bss));
HIDDEN(.Lape.bss.filesz = 0);
HIDDEN(.Lape.bss.memsz = SIZEOF(.bss));
HIDDEN(.Lape.bss.align = .Lape.data.align);

/* Program Loader Auto-Tune */
HIDDEN(v_ape_realsectors =
           MIN(REAL_SCRATCH_AREA - IMAGE_BASE_REAL,
               ROUNDUP(RVA(_edata), 4096)) / 512);
HIDDEN(v_ape_realpages = v_ape_realsectors / (4096 / 512));
HIDDEN(v_ape_highsectors =
           (ROUNDUP(RVA(_edata), 512) / 512) - v_ape_realsectors);

/* Windows NT Auto-Subsystem Embedding */
HIDDEN(v_ntsubsystem = (DEFINED(GetMessage)
                        ? kNtImageSubsystemWindowsGui
                        : kNtImageSubsystemWindowsCui));

/* ZIP End of Central Directory header */
#define ZIPCONST(NAME, VAL) HIDDEN(NAME = DEFINED(__zip_start) ? VAL : 0);
ZIPCONST(v_zip_cdoffset, __zip_start - IMAGE_BASE_VIRTUAL);
ZIPCONST(v_zip_cdirsize, __zip_end - __zip_start);
ZIPCONST(v_zip_records, v_zip_cdirsize / kZipCdirHdrLinkableSize);
ZIPCONST(v_zip_commentsize, _edata - __zip_end - kZipCdirHdrMinSize);

/* Generates deterministic ID for Mach-O. */
#define PHI 0x9e3779b9925d4c17
#define XOR(X,Y) ((X | Y) - (X & Y))
#define XORSHIFT(X,Y)    \
  X = XOR(X, (Y >> 12)); \
  X = XOR(X, (Y << 25)); \
  X = XOR(X, (Y >> 27))
#define KMH(X,Y)                       \
  X = (X + (Y >> 000) & 0xFF) * PHI; \
  X = (X + (Y >> 010) & 0xFF) * PHI; \
  X = (X + (Y >> 020) & 0xFF) * PHI; \
  X = (X + (Y >> 030) & 0xFF) * PHI
#define CHURN(X)        \
  XORSHIFT(uuid1_, X); \
  KMH(uuid1_, X);      \
  XORSHIFT(uuid2_, X); \
  KMH(uuid2_, X)

HIDDEN(uuid1_ = 88172645463325252);
HIDDEN(uuid2_ = 88172645463325252);

CHURN(.Lape.bss.align);
CHURN(.Lape.bss.filesz);
CHURN(.Lape.bss.memsz);
CHURN(.Lape.bss.offset);
CHURN(.Lape.bss.paddr);
CHURN(.Lape.data.align);
CHURN(.Lape.data.filesz);
CHURN(.Lape.data.memsz);
CHURN(.Lape.data.offset);
CHURN(.Lape.data.paddr);
CHURN(.Lape.data.rva);
CHURN(.Lape.data.vaddr);
CHURN(.Lape.elf.entry);
CHURN(.Lape.elf.phnum);
CHURN(.Lape.elf.phoff);
CHURN(.Lape.elf.shnum);
CHURN(.Lape.elf.shoff);
CHURN(.Lape.elf.shstrndx);
CHURN(.Lape.macho.end);
CHURN(.Lape.note);
CHURN(.Lape.note.align);
CHURN(.Lape.note.end);
CHURN(.Lape.note.filesz);
CHURN(.Lape.note.memsz);
CHURN(.Lape.note.offset);
CHURN(.Lape.note.paddr);
CHURN(.Lape.note.vaddr);
CHURN(.Lape.pe.offset);
CHURN(.Lape.pe.optsz);
CHURN(.Lape.pe.sections);
CHURN(.Lape.pe.sections_end);
CHURN(.Lape.pe.shnum);
CHURN(.Lape.phdrs.end);
CHURN(.Lape.ram.align);
CHURN(.Lape.ram.filesz);
CHURN(.Lape.ram.memsz);
CHURN(.Lape.ram.offset);
CHURN(.Lape.ram.paddr);
CHURN(.Lape.ram.rva);
CHURN(.Lape.ram.vaddr);
CHURN(.Lape.rom.align);
CHURN(.Lape.rom.filesz);
CHURN(.Lape.rom.memsz);
CHURN(.Lape.rom.offset);
CHURN(.Lape.rom.paddr);
CHURN(.Lape.rom.rva);
CHURN(.Lape.rom.vaddr);
CHURN(.Lape.text.align);
CHURN(.Lape.text.filesz);
CHURN(.Lape.text.memsz);
CHURN(.Lape.text.offset);
CHURN(.Lape.text.paddr);
CHURN(.Lape.text.rva);
CHURN(.Lape.text.vaddr);
CHURN(ADDR(.bss));
CHURN(WinMain);
CHURN(_start);
CHURN(ape.macho);
CHURN(ape.mz);
CHURN(ape.pe);
CHURN(ape.phdrs);
CHURN(v_ape_realsectors);

ASSERT(ape.mz == IMAGE_BASE_VIRTUAL, "linker panic");
ASSERT((DEFINED(__init_bss_end) ? __init_bss_end : 0) % __SIZEOF_POINTER__ == 0,
       "__init_bss misalign");
ASSERT(((DEFINED(__init_rodata_end) ? __init_rodata_end : 0) %
        __SIZEOF_POINTER__ == 0),
       "__init_rodata misalign");

ASSERT((!DEFINED(ape.grub) ? 1 : RVA(ape.grub) < 8192),
       "grub stub needs to be in first 8kb of image");

ASSERT(DEFINED(_start) || DEFINED(_start16),
       "please link a _start() or _start16() entrypoint");

ASSERT(!DEFINED(_start16) || REAL(_end) < 65536,
       "ape won't support non-tiny real mode programs");

/* Let's not be like Knight Capital. */
/* NOCROSSREFS_TO(.test .text) */

/* ASSERT(ape_sysv_start == .Lape.text.vaddr, */
/*        "ape_sysv_start() must be first in .text"); */

#endif /* __LINKER__ */
