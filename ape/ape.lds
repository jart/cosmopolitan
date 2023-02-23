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

    objcopy -S -O binary input.com.dbg output.com

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
#include "ape/relocations.h"
#include "libc/dce.h"
#include "libc/elf/def.h"
#include "libc/elf/pf2prot.internal.h"
#include "libc/nt/pedef.internal.h"
#include "libc/thread/tls.h"
#include "libc/zip.h"

ENTRY(_start)

PHDRS {
  Head PT_LOAD FLAGS(PF_X|PF_R);
  Rom PT_LOAD FLAGS(PF_X|PF_R);
  Ram PT_LOAD FLAGS(PF_W|PF_R);
  Tls PT_TLS FLAGS(PF_W|PF_R);
  Bss PT_LOAD FLAGS(PF_W|PF_R);
  stack PT_GNU_STACK FLAGS(PF_W|PF_R);
}

SECTIONS {

/*BEGIN: realmode addressability guarantee */
/*BEGIN: xnu addressability guarantee */
/*BEGIN: linux addressability guarantee */
/*BEGIN: bsd addressability guarantee */

  .head SEGMENT_START("text-segment", IMAGE_BASE_VIRTUAL) : AT(IMAGE_BASE_REAL) {
    HIDDEN(_base = .);

    /* Real Mode */
    KEEP(*(.head))
    KEEP(*(.apesh))
    KEEP(*(.head2))
    KEEP(*(.text.head))

    /* Executable & Linkable Format */
    . = ALIGN(__SIZEOF_POINTER__);
    HIDDEN(ape_phdrs = .);
    KEEP(*(.elf.phdrs))
    HIDDEN(ape_phdrs_end = .);

    KEEP(*(.emush))

    /* OpenBSD */
    . = ALIGN(__SIZEOF_POINTER__);
    HIDDEN(ape_note = .);
    KEEP(*(.note.openbsd.ident))
    KEEP(*(.note.netbsd.ident))
    HIDDEN(ape_note_end = .);

    /* Portable Executable */
    KEEP(*(.pe.header))
    HIDDEN(ape_pe_sections = .);
    KEEP(*(.pe.sections))
    HIDDEN(ape_pe_sections_end = .);

    /* Mach-O */
    KEEP(*(.macho))
    . = ALIGN(__SIZEOF_POINTER__);
    HIDDEN(ape_macho_end = .);

    KEEP(*(.ape.pad.head))
    . = ALIGN(SupportsWindows() || SupportsMetal() ? PAGESIZE : 16);
    HIDDEN(_ehead = .);
  } :Head

/*BEGIN: nt addressability guarantee */

  .text . : {
    BYTE(0x90) /* TODO: fix blinkenlights symbol __map_phdrs */
    /* Code that needs to be addressable in Real Mode */
    *(.text.real)
    KEEP(*(SORT_BY_NAME(.sort.text.real.*)))
    /* Code we want earlier in the binary w/o modifications */
    KEEP(*(.ape.loader))
    HIDDEN(_ereal = .);
/*END: realmode addressability guarantee */
/*BEGIN: morphable code */
    . += 1;

    /* Normal Code */
    *(.start)
    KEEP(*(.initprologue))
    KEEP(*(SORT_BY_NAME(.init.*)))
    KEEP(*(.init))
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
    KEEP(*(.blink))
    *(SORT_BY_ALIGNMENT(.text.modernity))
    *(SORT_BY_ALIGNMENT(.text.modernity.*))
    *(SORT_BY_ALIGNMENT(.text.hot))
    *(SORT_BY_ALIGNMENT(.text.hot.*))
    KEEP(*(.keep.text))
    *(.text .stub .text.*)
    KEEP(*(SORT_BY_NAME(.sort.text.*)))
    *(.subrs)

    KEEP(*(.ape.pad.test));
    *(.test.unlikely)
    *(.test .test.*)

    /* Privileged code invulnerable to magic */
    KEEP(*(.ape.pad.privileged));
    . += . > 0 ? 1 : 0;
/*END: morphable code */
    HIDDEN(__privileged_start = .);
    . += . > 0 ? 1 : 0;
    *(.privileged)
    HIDDEN(__privileged_end = .);
    . += . > 0 ? 1 : 0;

/*BEGIN: Read Only Data */

    KEEP(*(.ape.pad.rodata));
    KEEP(*(.rodata.pytab.0));
    KEEP(*(.rodata.pytab.1));
    KEEP(*(.rodata.pytab.2));

    *(.rodata .rodata.*)
    *(.ubsan.types)
    *(.ubsan.data)

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

    . = ALIGN(__SIZEOF_POINTER__);
    PROVIDE_HIDDEN(__init_array_start = .);
    KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*)
           SORT_BY_INIT_PRIORITY(.ctors.*)))
    KEEP(*(.ctors))
    KEEP(*(.init_array))
    KEEP(*(.preinit_array))
    PROVIDE_HIDDEN(__init_array_end = .);

    . = ALIGN(__SIZEOF_POINTER__);
    PROVIDE_HIDDEN(__fini_array_start = .);
    KEEP(*(SORT_BY_INIT_PRIORITY(.fini_array.*)
           SORT_BY_INIT_PRIORITY(.dtors.*)))
    KEEP(*(.dtors))
    PROVIDE_HIDDEN(__fini_array_end = .);

    /* Encoded Data Structures w/ Linear Initialization Order */
    KEEP(*(.initroprologue))
    KEEP(*(SORT_BY_NAME(.initro.*)))
    KEEP(*(.initroepilogue))
    KEEP(*(SORT_BY_NAME(.sort.rodata.*)))

    KEEP(*(.ape.pad.text))
    . = ALIGN(PAGESIZE);
    HIDDEN(_etext = .);
    PROVIDE_HIDDEN(etext = .);
/*END: Read Only Data (only needed for initialization) */
  } :Rom

/* initialization image for thread-local storage, this is copied */
/* out to actual TLS areas at runtime, so just make it read-only */
  .tdata . : {
    _tdata_start = .;
    *(SORT_BY_ALIGNMENT(.tdata))
    *(SORT_BY_ALIGNMENT(.tdata.*))
    _tdata_end = .;
    . = ALIGN(PAGESIZE);
  } :Tls :Rom
/*END: Read Only Data */

/* this only tells the linker about the layout of uninitialized */
/* TLS data, and does not advance the linker's location counter */
  .tbss . : {
    _tbss_start = .;
    *(SORT_BY_ALIGNMENT(.tbss))
    *(SORT_BY_ALIGNMENT(.tbss.*))
    . = ALIGN(TLS_ALIGNMENT);
    /* the %fs register is based on this location */
    _tbss_end = .;
  } :Tls

  .data ALIGN(PAGESIZE) : {
/*BEGIN: Read/Write Data */
    KEEP(*(SORT_BY_NAME(.piro.data.sort.iat.*)))
/*BEGIN: NT FORK COPYING */
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
    . = ALIGN(__SIZEOF_POINTER__);
    KEEP(*(SORT_BY_NAME(.piro.relo.sort.*)))
    PROVIDE_HIDDEN(__relo_end = .);
    . = ALIGN(__SIZEOF_POINTER__);
    KEEP(*(SORT_BY_NAME(.piro.data.sort.*)))
    KEEP(*(.piro.pad.data))
    KEEP(*(.dataepilogue))
/*END: NT FORK COPYING */
    HIDDEN(_edata = .);
    PROVIDE_HIDDEN(edata = .);
    KEEP(*(SORT_BY_NAME(.zip.*)))
    HIDDEN(_ezip = .);
    . = ALIGN(PAGESIZE);
  } :Ram

/*END:   file content that's loaded by o/s */
/*END:   file content */
/*BEGIN: bss memory that's addressable */

  .bss ALIGN(64) : {
/*BEGIN: NT FORK COPYING */
    KEEP(*(.bssprologue))
    KEEP(*(SORT_BY_NAME(.piro.bss.init.*)))
    *(.piro.bss)
    KEEP(*(SORT_BY_NAME(.piro.bss.sort.*)))
    HIDDEN(__piro_end = .);
    . += . > 0 ? 1 : 0;
/*END: Post-Initialization Read-Only */

    /* Statically Allocated Empty Space */
    *(SORT_BY_ALIGNMENT(.bss))
    *(SORT_BY_ALIGNMENT(.bss.*))
    *(COMMON)

    KEEP(*(SORT_BY_NAME(.sort.bss.*)))

    KEEP(*(.bssepilogue))
/*END: NT FORK COPYING */
    . = ALIGN(PAGESIZE);
    HIDDEN(_end = .);
    PROVIDE_HIDDEN(end = .);
  } :Bss

/*END: nt addressability guarantee */
/*END: bsd addressability guarantee */
/*END: linux addressability guarantee */
/*END: xnu addressability guarantee */

  .shstrtab            : { *(.shstrtab) }
  .strtab              : { *(.strtab) }
  .symtab              : { *(.symtab) }
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
  .debug_line_str    0 : { *(.debug_line_str) }
  .debug_loc         0 : { *(.debug_loc) }
  .debug_macinfo     0 : { *(.debug_macinfo) }
  .debug_weaknames   0 : { *(.debug_weaknames) }
  .debug_funcnames   0 : { *(.debug_funcnames) }
  .debug_typenames   0 : { *(.debug_typenames) }
  .debug_varnames    0 : { *(.debug_varnames) }
  .debug_pubtypes    0 : { *(.debug_pubtypes) }
  .debug_ranges      0 : { *(.debug_ranges) }
  .debug_rnglists    0 : { *(.debug_rnglists) }
  .debug_macro       0 : { *(.debug_macro) }
  .debug_addr        0 : { *(.debug_addr) }
  .gnu.attributes    0 : { KEEP(*(.gnu.attributes)) }
  .GCC.command.line  0 : { *(.GCC.command.line) }

  /DISCARD/ : {
    *(__mcount_loc)
    *(.discard)
    *(.yoink)
    *(.*)
  }
}

PFSTUB8(ape_elf_entry, _start);
PFSTUB8(ape_elf_phoff, RVA(ape_phdrs));
PFSTUB8(ape_elf_shoff, 0);
PFSTUB4(ape_elf_phnum, (ape_phdrs_end - ape_phdrs) / 56);
PFSTUB4(ape_elf_shnum, 0);
PFSTUB4(ape_elf_shstrndx, 0);

HIDDEN(_tls_size = _tbss_end - _tdata_start);
HIDDEN(_tdata_size = _tdata_end - _tdata_start);
HIDDEN(_tbss_size = _tbss_end - _tbss_start);
HIDDEN(_tbss_offset = _tbss_start - _tdata_start);
HIDDEN(_tls_content = (_tdata_end - _tdata_start) + (_tbss_end - _tbss_start));

HIDDEN(__privileged_addr = ROUNDDOWN(__privileged_start, PAGESIZE));
HIDDEN(__privileged_size = (ROUNDUP(__privileged_end, PAGESIZE) -
                            ROUNDDOWN(__privileged_start, PAGESIZE)));

HIDDEN(ape_rom_offset = 0);
HIDDEN(ape_rom_vaddr = ADDR(.head));
HIDDEN(ape_rom_paddr = LOADADDR(.head));
HIDDEN(ape_rom_filesz = LOADADDR(.data) - ape_rom_paddr);
HIDDEN(ape_rom_memsz = ADDR(.data) - ADDR(.head));
HIDDEN(ape_rom_align = PAGESIZE);
HIDDEN(ape_rom_rva = RVA(ape_rom_vaddr));

HIDDEN(ape_ram_offset = ape_rom_offset + ape_rom_filesz);
HIDDEN(ape_ram_vaddr = ADDR(.data));
HIDDEN(ape_ram_paddr = LOADADDR(.data));
HIDDEN(ape_ram_filesz = SIZEOF(.data));
HIDDEN(ape_ram_memsz = ADDR(.bss) + SIZEOF(.bss) - ape_ram_vaddr);
HIDDEN(ape_ram_align = PAGESIZE);
HIDDEN(ape_ram_rva = RVA(ape_ram_vaddr));

HIDDEN(ape_stack_pf = DEFINED(ape_stack_pf) ? ape_stack_pf : PF_R | PF_W);
HIDDEN(ape_stack_prot = _PF2PROT(ape_stack_pf));
HIDDEN(ape_stack_offset = ape_ram_offset + ape_ram_filesz);
HIDDEN(ape_stack_vaddr = DEFINED(ape_stack_vaddr) ? ape_stack_vaddr : 0x700000000000);
HIDDEN(ape_stack_paddr = ape_ram_paddr + ape_ram_filesz);
HIDDEN(ape_stack_filesz = 0);
HIDDEN(ape_stack_memsz = DEFINED(ape_stack_memsz) ? ape_stack_memsz : STACKSIZE);
HIDDEN(ape_stack_align = 16);

HIDDEN(ape_note_offset = ape_rom_offset + (ape_note - ape_rom_vaddr));
HIDDEN(ape_note_vaddr = ape_note);
HIDDEN(ape_note_paddr = ape_rom_paddr + ape_note_offset);
HIDDEN(ape_note_filesz = ape_note_end - ape_note);
HIDDEN(ape_note_memsz = ape_note_filesz);
HIDDEN(ape_note_align = __SIZEOF_POINTER__);

HIDDEN(ape_text_offset = ape_rom_offset + LOADADDR(.text) - ape_rom_paddr);
HIDDEN(ape_text_paddr = LOADADDR(.text));
HIDDEN(ape_text_vaddr = ADDR(.text));
HIDDEN(ape_text_filesz = SIZEOF(.text) + SIZEOF(.tdata));
HIDDEN(ape_text_memsz = SIZEOF(.text) + SIZEOF(.tdata));
HIDDEN(ape_text_align = PAGESIZE);
HIDDEN(ape_text_rva = RVA(ape_text_vaddr));

HIDDEN(ape_data_offset = ape_ram_offset + LOADADDR(.data) - ape_ram_paddr);
HIDDEN(ape_data_paddr = LOADADDR(.data));
HIDDEN(ape_data_vaddr = ADDR(.data));
HIDDEN(ape_data_filesz = SIZEOF(.data));
HIDDEN(ape_data_memsz = SIZEOF(.data));
HIDDEN(ape_data_align = PAGESIZE);
HIDDEN(ape_data_rva = RVA(ape_data_vaddr));

HIDDEN(ape_bss_offset = ape_ram_offset + LOADADDR(.bss) - ape_ram_paddr);
HIDDEN(ape_bss_paddr = LOADADDR(.bss));
HIDDEN(ape_bss_vaddr = ADDR(.bss));
HIDDEN(ape_bss_filesz = 0);
HIDDEN(ape_bss_memsz = SIZEOF(.bss));
HIDDEN(ape_bss_align = PAGESIZE);

/* we roundup here because xnu wants the file load segments page-aligned */
/* but we don't want to add the nop padding to the ape program, so we'll */
/* let ape.S dd read past the end of the file into the wrapping binaries */
SHSTUB2(ape_loader_dd_skip, DEFINED(ape_loader) ? RVA(ape_loader) / 64 : 0);
SHSTUB2(ape_loader_dd_count,
        DEFINED(ape_loader_end)
            ? ROUNDUP(ape_loader_end - ape_loader, PAGESIZE) / 64
            : 0);

HIDDEN(blink_aarch64_b0 = RVA(blink_aarch64) % 10 + 48);
HIDDEN(blink_aarch64_b1 = RVA(blink_aarch64) < 10 ? 32 : RVA(blink_aarch64) / 10 % 10 + 48);
HIDDEN(blink_aarch64_b2 = RVA(blink_aarch64) < 100 ? 32 : RVA(blink_aarch64) / 100 % 10 + 48);
HIDDEN(blink_aarch64_b3 = RVA(blink_aarch64) < 1000 ? 32 : RVA(blink_aarch64) / 1000 % 10 + 48);
HIDDEN(blink_aarch64_b4 = RVA(blink_aarch64) < 10000 ? 32 : RVA(blink_aarch64) / 10000 % 10 + 48);
HIDDEN(blink_aarch64_b5 = RVA(blink_aarch64) < 100000 ? 32 : RVA(blink_aarch64) / 100000 % 10 + 48);
HIDDEN(blink_aarch64_b6 = RVA(blink_aarch64) < 1000000 ? 32 : RVA(blink_aarch64) / 1000000 % 10 + 48);
HIDDEN(blink_aarch64_b7 = RVA(blink_aarch64) < 10000000 ? 32 : RVA(blink_aarch64) / 10000000 % 10 + 48);
HIDDEN(blink_aarch64_b8 = RVA(blink_aarch64) < 100000000 ? 32 : RVA(blink_aarch64) / 100000000 % 10 + 48);
HIDDEN(blink_aarch64_b9 = RVA(blink_aarch64) < 1000000000 ? 32 : RVA(blink_aarch64) / 1000000000 % 10 + 48);

HIDDEN(blink_aarch64_size_b0 = blink_aarch64_size % 10 + 48);
HIDDEN(blink_aarch64_size_b1 = blink_aarch64_size < 10 ? 32 : blink_aarch64_size / 10 % 10 + 48);
HIDDEN(blink_aarch64_size_b2 = blink_aarch64_size < 100 ? 32 : blink_aarch64_size / 100 % 10 + 48);
HIDDEN(blink_aarch64_size_b3 = blink_aarch64_size < 1000 ? 32 : blink_aarch64_size / 1000 % 10 + 48);
HIDDEN(blink_aarch64_size_b4 = blink_aarch64_size < 10000 ? 32 : blink_aarch64_size / 10000 % 10 + 48);
HIDDEN(blink_aarch64_size_b5 = blink_aarch64_size < 100000 ? 32 : blink_aarch64_size / 100000 % 10 + 48);
HIDDEN(blink_aarch64_size_b6 = blink_aarch64_size < 1000000 ? 32 : blink_aarch64_size / 1000000 % 10 + 48);
HIDDEN(blink_aarch64_size_b7 = blink_aarch64_size < 10000000 ? 32 : blink_aarch64_size / 10000000 % 10 + 48);
HIDDEN(blink_aarch64_size_b8 = blink_aarch64_size < 100000000 ? 32 : blink_aarch64_size / 100000000 % 10 + 48);
HIDDEN(blink_aarch64_size_b9 = blink_aarch64_size < 1000000000 ? 32 : blink_aarch64_size / 1000000000 % 10 + 48);

HIDDEN(blink_darwin_arm64_b0 = RVA(blink_darwin_arm64) % 10 + 48);
HIDDEN(blink_darwin_arm64_b1 = RVA(blink_darwin_arm64) < 10 ? 32 : RVA(blink_darwin_arm64) / 10 % 10 + 48);
HIDDEN(blink_darwin_arm64_b2 = RVA(blink_darwin_arm64) < 100 ? 32 : RVA(blink_darwin_arm64) / 100 % 10 + 48);
HIDDEN(blink_darwin_arm64_b3 = RVA(blink_darwin_arm64) < 1000 ? 32 : RVA(blink_darwin_arm64) / 1000 % 10 + 48);
HIDDEN(blink_darwin_arm64_b4 = RVA(blink_darwin_arm64) < 10000 ? 32 : RVA(blink_darwin_arm64) / 10000 % 10 + 48);
HIDDEN(blink_darwin_arm64_b5 = RVA(blink_darwin_arm64) < 100000 ? 32 : RVA(blink_darwin_arm64) / 100000 % 10 + 48);
HIDDEN(blink_darwin_arm64_b6 = RVA(blink_darwin_arm64) < 1000000 ? 32 : RVA(blink_darwin_arm64) / 1000000 % 10 + 48);
HIDDEN(blink_darwin_arm64_b7 = RVA(blink_darwin_arm64) < 10000000 ? 32 : RVA(blink_darwin_arm64) / 10000000 % 10 + 48);
HIDDEN(blink_darwin_arm64_b8 = RVA(blink_darwin_arm64) < 100000000 ? 32 : RVA(blink_darwin_arm64) / 100000000 % 10 + 48);
HIDDEN(blink_darwin_arm64_b9 = RVA(blink_darwin_arm64) < 1000000000 ? 32 : RVA(blink_darwin_arm64) / 1000000000 % 10 + 48);

HIDDEN(blink_darwin_arm64_size_b0 = blink_darwin_arm64_size % 10 + 48);
HIDDEN(blink_darwin_arm64_size_b1 = blink_darwin_arm64_size < 10 ? 32 : blink_darwin_arm64_size / 10 % 10 + 48);
HIDDEN(blink_darwin_arm64_size_b2 = blink_darwin_arm64_size < 100 ? 32 : blink_darwin_arm64_size / 100 % 10 + 48);
HIDDEN(blink_darwin_arm64_size_b3 = blink_darwin_arm64_size < 1000 ? 32 : blink_darwin_arm64_size / 1000 % 10 + 48);
HIDDEN(blink_darwin_arm64_size_b4 = blink_darwin_arm64_size < 10000 ? 32 : blink_darwin_arm64_size / 10000 % 10 + 48);
HIDDEN(blink_darwin_arm64_size_b5 = blink_darwin_arm64_size < 100000 ? 32 : blink_darwin_arm64_size / 100000 % 10 + 48);
HIDDEN(blink_darwin_arm64_size_b6 = blink_darwin_arm64_size < 1000000 ? 32 : blink_darwin_arm64_size / 1000000 % 10 + 48);
HIDDEN(blink_darwin_arm64_size_b7 = blink_darwin_arm64_size < 10000000 ? 32 : blink_darwin_arm64_size / 10000000 % 10 + 48);
HIDDEN(blink_darwin_arm64_size_b8 = blink_darwin_arm64_size < 100000000 ? 32 : blink_darwin_arm64_size / 100000000 % 10 + 48);
HIDDEN(blink_darwin_arm64_size_b9 = blink_darwin_arm64_size < 1000000000 ? 32 : blink_darwin_arm64_size / 1000000000 % 10 + 48);

#if SupportsMetal()
HIDDEN(v_ape_realsectors =
           MIN(0x70000 - IMAGE_BASE_REAL, ROUNDUP(RVA(_ezip), 512)) / 512);
HIDDEN(v_ape_realbytes = v_ape_realsectors * 512);
HIDDEN(v_ape_realdwords = v_ape_realsectors * (512 / 4));
HIDDEN(v_ape_allsectors = ROUNDUP(RVA(_ezip), 512) / 512);
HIDDEN(v_ape_allbytes = v_ape_allsectors * 512);
HIDDEN(v_ape_highsectors = MIN(0xffff, v_ape_allsectors - v_ape_realsectors));
TSSDESCSTUB2(_tss, _tss, _tss_end ? _tss_end - _tss - 1 : 0);
#endif

/* ZIP End of Central Directory header */
#define ZIPCONST(NAME, VAL) HIDDEN(NAME = DEFINED(__zip_start) ? VAL : 0);
ZIPCONST(v_zip_cdoffset, __zip_start - IMAGE_BASE_VIRTUAL);
ZIPCONST(v_zip_cdirsize, __zip_end - __zip_start);
ASSERT(v_zip_cdirsize % kZipCdirHdrLinkableSize == 0, "bad zip cdir");
ZIPCONST(v_zip_records, v_zip_cdirsize / kZipCdirHdrLinkableSize);
ZIPCONST(v_zip_commentsize, _ezip - __zip_end - kZipCdirHdrMinSize);

#if SupportsXnu()
/* Generates deterministic ID. */
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
  XORSHIFT(ape_uuid1, X); \
  KMH(ape_uuid1, X);      \
  XORSHIFT(ape_uuid2, X); \
  KMH(ape_uuid2, X)
HIDDEN(ape_uuid1 = 88172645463325252);
HIDDEN(ape_uuid2 = 88172645463325252);
CHURN(ape_bss_align);
CHURN(ape_bss_filesz);
CHURN(ape_bss_memsz);
CHURN(ape_bss_offset);
CHURN(ape_bss_paddr);
CHURN(ape_data_filesz);
CHURN(ape_data_memsz);
CHURN(ape_data_offset);
CHURN(ape_data_paddr);
CHURN(ape_data_rva);
CHURN(ape_data_vaddr);
CHURN(ape_elf_entry);
CHURN(ape_elf_phnum);
CHURN(ape_elf_phoff);
CHURN(ape_elf_shnum);
CHURN(ape_elf_shoff);
CHURN(ape_elf_shstrndx);
CHURN(ape_macho_end);
CHURN(ape_note);
CHURN(ape_note_align);
CHURN(ape_note_end);
CHURN(ape_note_filesz);
CHURN(ape_note_memsz);
CHURN(ape_note_offset);
CHURN(ape_note_paddr);
CHURN(ape_note_vaddr);
CHURN(ape_ram_align);
CHURN(ape_ram_filesz);
CHURN(ape_ram_memsz);
CHURN(ape_ram_offset);
CHURN(ape_ram_paddr);
CHURN(ape_ram_rva);
CHURN(ape_ram_vaddr);
CHURN(ape_rom_align);
CHURN(ape_rom_filesz);
CHURN(ape_rom_memsz);
CHURN(ape_rom_offset);
CHURN(ape_rom_paddr);
CHURN(ape_rom_rva);
CHURN(ape_rom_vaddr);
CHURN(ape_text_align);
CHURN(ape_text_filesz);
CHURN(ape_text_memsz);
CHURN(ape_text_offset);
CHURN(ape_text_paddr);
CHURN(ape_text_rva);
CHURN(ape_text_vaddr);
CHURN(ADDR(.bss));
CHURN(_start);
CHURN(ape_phdrs);
#if SupportsMetal()
CHURN(v_ape_allsectors);
#endif
#if SupportsXnu()
CHURN(ape_macho);
#endif
#if SupportsWindows() || SupportsMetal()
CHURN(ape_mz);
CHURN(ape_pe);
CHURN(ape_pe_offset);
CHURN(ape_pe_optsz);
CHURN(ape_pe_sections);
CHURN(ape_pe_sections_end);
CHURN(ape_pe_shnum);
CHURN(ape_phdrs_end);
CHURN(WinMain);
#endif /* SupportsWindows() */
#endif /* SupportsXnu() */

#if SupportsWindows() || SupportsMetal()
#define LINK_WINDOWS (SupportsWindows() && !DEFINED(EfiMain))
PFSTUB4(ape_pe_offset, ape_pe - ape_mz);
HIDDEN(ape_pe_optsz = ape_pe_sections - (ape_pe + 24));
HIDDEN(ape_pe_shnum = (ape_pe_sections_end - ape_pe_sections) / 40);
HIDDEN(ape_pe_base = IMAGE_BASE_VIRTUAL);
HIDDEN(ape_idata = LINK_WINDOWS ? RVA(ape_idata_iat) : 0);
HIDDEN(ape_idata_iatsize = LINK_WINDOWS ? ape_idata_iatend - ape_idata_iat : 0);
HIDDEN(ape_idata = LINK_WINDOWS ? RVA(ape_idata_idt) : 0);
HIDDEN(ape_idata_idtsize = LINK_WINDOWS ? ape_idata_idtend - ape_idata_idt : 0);
HIDDEN(v_ntversion = LINK_WINDOWS ? 6 : 1);
HIDDEN(v_ntdllchar = LINK_WINDOWS ? 288 : 0);
HIDDEN(v_ntsubversion = LINK_WINDOWS ? 6 : 5);
HIDDEN(v_ntsubsystem = (LINK_WINDOWS
                        ? (DEFINED(GetMessage)
                           ? kNtImageSubsystemWindowsGui
                           : kNtImageSubsystemWindowsCui)
                        : kNtImageSubsystemEfiApplication));
HIDDEN(ape_pe_entry = LINK_WINDOWS ? WinMain : EfiMain);
#endif

#if SupportsXnu()
SHSTUB2(ape_macho_dd_skip, RVA(ape_macho) / 8);
SHSTUB2(ape_macho_dd_count, (ape_macho_end - ape_macho) / 8);
#endif

ASSERT(DEFINED(ape_mz) ? ape_mz == IMAGE_BASE_VIRTUAL : 1, "linker panic");
ASSERT((DEFINED(__init_bss_end) ? __init_bss_end : 0) % __SIZEOF_POINTER__ == 0,
       "__init_bss misalign");
ASSERT(((DEFINED(__init_rodata_end) ? __init_rodata_end : 0) %
        __SIZEOF_POINTER__ == 0),
       "__init_rodata misalign");

ASSERT((!DEFINED(ape_grub) ? 1 : RVA(ape_grub) < 8192),
       "grub stub needs to be in first 8kb of image");

ASSERT(DEFINED(_start) || DEFINED(_start16),
       "please link a _start() or _start16() entrypoint");

ASSERT(!DEFINED(_start16) || REAL(_end) < 65536,
       "ape won't support non-tiny real mode programs");

ASSERT(IS2POW(ape_stack_memsz),
       "ape_stack_memsz must be a two power");

ASSERT(ape_stack_vaddr % ape_stack_memsz == 0,
       "ape_stack_vaddr must have ape_stack_memsz alignment; try using STATIC_STACK_ADDR(0x700000040000 & -ape_stack_memsz);");

ASSERT(ALIGNOF(.tdata) <= TLS_ALIGNMENT && ALIGNOF(.tbss) <= TLS_ALIGNMENT,
       "_Thread_local _Alignof can't exceed TLS_ALIGNMENT");

/* Let's not be like Knight Capital. */
/* NOCROSSREFS_TO(.test .text) */

/* ASSERT(ape_sysv_start == ape_text_vaddr, */
/*        "ape_sysv_start() must be first in .text"); */

#endif /* __LINKER__ */
