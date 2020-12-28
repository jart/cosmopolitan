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
╚─────────────────────────────────────────────────────────────────────────────*/
#include "ape/relocations.h"

/**
 * @fileoverview Cosmopolitan ELF-Normative Linker Script.
 *
 * This linker script is an alternative to αcτµαlly pδrταblε εxεcµταblε.
 * It's intended for host-only binaries that don't need to be able to run
 * without an operating system, and don't need to be distributed. These
 * are still static executables, and they start at roughly 1kb in size.
 *
 * @see ape/ape.lds
 */

ENTRY(_start)

SECTIONS {
  . = SEGMENT_START("text-segment", IMAGE_BASE_VIRTUAL) + SIZEOF_HEADERS;

  .real : {
    HIDDEN(_base = .);
    *(.text.real)
    HIDDEN(_ereal = .);
  }

  .start : {
    *(.start)
    KEEP(*(.initprologue))
    KEEP(*(SORT_BY_NAME(.init.*)))
    KEEP(*(SORT_NONE(.init)))
    KEEP(*(.initepilogue))
    *(.text.startup)
  }

  .exit : {
    *(.text.exit)
  }

  .cold : {
    *(.text.unlikely)
  }

  .windows : {
    *(.textwindowsprologue)
    *(.text.windows)  /* ← you can flip the windows bit in -DSUPPORT_VECTOR */
    *(.textwindowsepilogue)
  }

  .text : {
    *(SORT_BY_ALIGNMENT(.text.hot))
    KEEP(*(.keep.text))
    *(.text .text.*)
    KEEP(*(SORT_BY_NAME(.sort.text.*)))
  }

  .test : {
    HIDDEN(__test_start = .);
    *(.test.unlikely)
    *(.test .test.*)
  }

  .privileged : {
    . = ALIGN(DEFINED(ftrace_init) ? PAGESIZE : 1);
    HIDDEN(__privileged_start = .);
    *(.privileged)
  }

  .rodata : {
    HIDDEN(__ro = .); /* ←for gdb readibility */

    /* Read-Only Data */
    *(.rodata .rodata.*)
    KEEP(*(SORT_BY_NAME(.sort.rodata.*)))

    /* DSO stuff */
    PROVIDE_HIDDEN(__got_plt = .);
    *(.got.plt)

    /* Windows DLL Import Directory */
    HIDDEN(idata.ro = .);
    KEEP(*(SORT_BY_NAME(.idata.ro.*)))
  }

  .initro : {
    /* Encoded Data Structures */
    KEEP(*(.initroprologue))
    KEEP(*(SORT_BY_NAME(.initro.*)))
    KEEP(*(.initroepilogue))
  }

  .ubsan : {
    /* Undefined Behavior Sanitizer Types */
    PROVIDE_HIDDEN(__ubsan_types_start = .);
    *(.ubsan.types)
    PROVIDE_HIDDEN(__ubsan_types_end = .);
    PROVIDE_HIDDEN(__ubsan_data_start = .);
    *(.ubsan.data)
    PROVIDE_HIDDEN(__ubsan_data_end = .);
  }

  HIDDEN(_etext = .);
  PROVIDE_HIDDEN(etext = .);

  . = DATA_SEGMENT_ALIGN(CONSTANT(MAXPAGESIZE), CONSTANT(COMMONPAGESIZE));

  .data : {
    *(.data .data.*)
    KEEP(*(SORT_BY_NAME(.sort.data.*)))
    SORT(CONSTRUCTORS)
    KEEP(*(.edata))
    . = ALIGN(__SIZEOF_POINTER__);
    HIDDEN(__piro_start = .);
    QUAD(IMAGE_BASE_VIRTUAL);
    PROVIDE_HIDDEN(__init_array_start = .);
    KEEP(*(.preinit_array))
    KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*)
           SORT_BY_INIT_PRIORITY(.ctors.*)))
    KEEP(*(SORT_NONE(.init_array)))
    KEEP(*(SORT_NONE(.ctors)))
    PROVIDE_HIDDEN(__init_array_end = .);
    PROVIDE_HIDDEN(__fini_array_start = .);
    KEEP(*(SORT_BY_INIT_PRIORITY(.fini_array.*)
           SORT_BY_INIT_PRIORITY(.dtors.*)))
    KEEP(*(SORT_NONE(.fini_array)))
    KEEP(*(SORT_NONE(.dtors)))
    PROVIDE_HIDDEN(__fini_array_end = .);
    KEEP(*(SORT_BY_NAME(.piro.relo.sort.*)))
    *(.data.rel.ro .data.rel.ro.*)
    PROVIDE_HIDDEN(__relo_end = .);
    KEEP(*(SORT_BY_NAME(.piro.data.sort.*)))
  }

  HIDDEN(_edata = .);
  PROVIDE_HIDDEN(edata = .);

  .bss : {
    KEEP(*(SORT_BY_NAME(.piro.bss.init.*)))
    *(.piro.bss)
    KEEP(*(SORT_BY_NAME(.piro.bss.sort.*)))
    HIDDEN(__piro_end = .);
    *(.bss .bss.*)
    *(COMMON)
    KEEP(*(SORT_BY_NAME(.sort.bss.*)))
    . = ALIGN(ABSOLUTE(.) - IMAGE_BASE_VIRTUAL > 0x80000000 ? 0x80000000 :
              ABSOLUTE(.) - IMAGE_BASE_VIRTUAL > 0x200000 ? 0x200000 : 0x1000);
    *(.xlm)
    . = ALIGN(ABSOLUTE(.) - IMAGE_BASE_VIRTUAL > 0x80000000 ? 0x80000000 :
              ABSOLUTE(.) - IMAGE_BASE_VIRTUAL > 0x200000 ? 0x200000 : 0x1000);
    . = ALIGN(. != 0 ? __SIZEOF_POINTER__ : 1);
  }

  HIDDEN(_end = .);
  PROVIDE_HIDDEN(end = .);

  .gnu_debuglink    0 : { *(.gnu_debuglink) }
  .stab             0 : { *(.stab) }
  .stabstr          0 : { *(.stabstr) }
  .stab.excl        0 : { *(.stab.excl) }
  .stab.exclstr     0 : { *(.stab.exclstr) }
  .stab.index       0 : { *(.stab.index) }
  .stab.indexstr    0 : { *(.stab.indexstr) }
  .debug            0 : { *(.debug) }
  .line             0 : { *(.line) }
  .debug_srcinfo    0 : { *(.debug_srcinfo) }
  .debug_sfnames    0 : { *(.debug_sfnames) }
  .debug_aranges    0 : { *(.debug_aranges) }
  .debug_pubnames   0 : { *(.debug_pubnames) }
  .debug_info       0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev     0 : { *(.debug_abbrev) }
  .debug_line       0 : { *(.debug_line .debug_line.* .debug_line_end ) }
  .debug_frame      0 : { *(.debug_frame) }
  .debug_str        0 : { *(.debug_str) }
  .debug_loc        0 : { *(.debug_loc) }
  .debug_macinfo    0 : { *(.debug_macinfo) }
  .debug_weaknames  0 : { *(.debug_weaknames) }
  .debug_funcnames  0 : { *(.debug_funcnames) }
  .debug_typenames  0 : { *(.debug_typenames) }
  .debug_varnames   0 : { *(.debug_varnames) }
  .debug_pubtypes   0 : { *(.debug_pubtypes) }
  .debug_ranges     0 : { *(.debug_ranges) }
  .debug_macro      0 : { *(.debug_macro) }
  .debug_addr       0 : { *(.debug_addr) }
  .gnu.attributes   0 : { KEEP(*(.gnu.attributes)) }
  .GCC.command.line 0 : { *(.GCC.command.line) }

  /DISCARD/ : {
    *(.comment)
    *(.discard)
    *(.yoink)
    *(.*)
  }
}
