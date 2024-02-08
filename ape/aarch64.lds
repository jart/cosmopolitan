/*-*- mode: ld-script; indent-tabs-mode: nil; tab-width: 2; coding: utf-8   -*-│
│ vi: set et sts=2 sw=2 fenc=utf-8                                         :vi │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/thread/tls.h"

ENTRY(_start)
OUTPUT_ARCH(aarch64)
OUTPUT_FORMAT("elf64-littleaarch64",
              "elf64-bigaarch64",
              "elf64-littleaarch64")

SECTIONS {

  . = SEGMENT_START("text-segment", 0x010000000000);
  __executable_start = .;
  . += SIZEOF_HEADERS;

  .interp            : { *(.interp) }
  .note.gnu.build-id : { *(.note.gnu.build-id) }
  .hash              : { *(.hash) }
  .gnu.hash          : { *(.gnu.hash) }
  .dynsym            : { *(.dynsym) }
  .dynstr            : { *(.dynstr) }
  .gnu.version       : { *(.gnu.version) }
  .gnu.version_d     : { *(.gnu.version_d) }
  .gnu.version_r     : { *(.gnu.version_r) }
  .rela.init         : { *(.rela.init) }
  .rela.text         : { *(.rela.text .rela.text.* .rela.gnu.linkonce.t.*) }
  .rela.fini         : { *(.rela.fini) }
  .rela.rodata       : { *(.rela.rodata .rela.rodata.* .rela.gnu.linkonce.r.*) }
  .rela.data.rel.ro  : { *(.rela.data.rel.ro .rela.data.rel.ro.* .rela.gnu.linkonce.d.rel.ro.*) }
  .rela.data         : { *(.rela.data .rela.data.* .rela.gnu.linkonce.d.*) }
  .rela.tdata        : { *(.rela.tdata .rela.tdata.* .rela.gnu.linkonce.td.*) }
  .rela.tbss         : { *(.rela.tbss .rela.tbss.* .rela.gnu.linkonce.tb.*) }
  .rela.ctors        : { *(.rela.ctors) }
  .rela.dtors        : { *(.rela.dtors) }
  .rela.got          : { *(.rela.got) }
  .rela.bss          : { *(.rela.bss .rela.bss.* .rela.gnu.linkonce.b.*) }
  .rela.ifunc        : { *(.rela.ifunc) }

  .rela.plt : {
    *(.rela.plt)
    __rela_iplt_start = .;
    *(.rela.iplt)
    __rela_iplt_end = .;
  }

  .init : {
    *(.start)
    KEEP(*(.initprologue))
    KEEP(*(SORT_NONE(.init)))
    KEEP(*(.initepilogue))
  } =0x1f2003d5

  .plt : ALIGN(16) {
    *(.plt)
    *(.iplt)
  }

  .text : {
    *(.text.unlikely .text.*_unlikely .text.unlikely.*)
    *(.text.antiquity .text.antiquity.*)
    *(.text.exit .text.exit.*)
    *(.text.startup .text.startup.*)
    *(.text.hot .text.hot.*)
    *(.text.modernity .text.modernity.*)
    *(.text .stub .text.* .gnu.linkonce.t.*)
    *(.gnu.warning)
  } =0x1f2003d5

  .fini : {
    KEEP(*(SORT_NONE(.fini)))
  } =0x1f2003d5

  . += CONSTANT(MAXPAGESIZE);

  .privileged : {
    __privileged_start = ABSOLUTE(.) & -CONSTANT(MAXPAGESIZE);
    *(.privileged*)
  } =0x1f2003d6

  .rodata : {
    KEEP(*(.rodata.pytab.0));
    KEEP(*(.rodata.pytab.1));
    KEEP(*(.rodata.pytab.2));
    KEEP(*(SORT_BY_NAME(.sort.rodata.*)))
    *(.rodata .rodata.* .gnu.linkonce.r.*)
    *(.ubsan.types)
    *(.ubsan.data)
  }

  .comment : {
    __comment_start = .;
    KEEP(*(.comment))
    BYTE(0);
  }

  .eh_frame_hdr : {
    *(.eh_frame_hdr)
    *(.eh_frame_entry .eh_frame_entry.*)
  }

  .eh_frame : ONLY_IF_RO {
    KEEP(*(.eh_frame))
    *(.eh_frame.*)
  }

  .gcc_except_table : ONLY_IF_RO {
    *(.gcc_except_table .gcc_except_table.*)
  }

  .gnu_extab : ONLY_IF_RO {
    *(.gnu_extab*)
  }

  .exception_ranges : ONLY_IF_RO {
    *(.exception_ranges*)
  }

  __etext = .;
  _etext = .;
  PROVIDE(etext = .);

  . += CONSTANT(MAXPAGESIZE);
  . = DATA_SEGMENT_ALIGN(CONSTANT(MAXPAGESIZE), CONSTANT(COMMONPAGESIZE));

  .eh_frame : ONLY_IF_RW {
    KEEP(*(.eh_frame))
    *(.eh_frame.*)
  }

  .gnu_extab : ONLY_IF_RW {
    *(.gnu_extab)
  }

  .gcc_except_table : ONLY_IF_RW {
    *(.gcc_except_table .gcc_except_table.*)
  }

  .exception_ranges : ONLY_IF_RW {
    *(.exception_ranges*)
  }

  .tdata : {
    _tdata_start = .;
    __tdata_start = .;
    *(.tdata .tdata.* .gnu.linkonce.td.*)
    _tdata_end = .;
  }

  .tbss : {
    _tbss_start = .;
    *(.tbss .tbss.* .gnu.linkonce.tb.*)
    *(.tcommon)
    _tbss_end = .;
  }

  .init_array : {
    __init_array_start = .;
    KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
    KEEP(*(.init_array EXCLUDE_FILE(*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors))
    __init_array_end = .;
  }

  .fini_array : {
    __fini_array_start = .;
    KEEP(*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
    KEEP(*(.fini_array EXCLUDE_FILE(*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .dtors))
    __fini_array_end = .;
  }

  .data.rel.ro : {
    KEEP(*(SORT_BY_NAME(.piro.relo.sort.*)))
    *(.data.rel.ro.local* .gnu.linkonce.d.rel.ro.local.*)
    *(.data.rel.ro .data.rel.ro.* .gnu.linkonce.d.rel.ro.*)
  }

  .dynamic : {
    *(.dynamic)
  }

  .got : {
    *(.got)
    *(.igot)
  }

  . = DATA_SEGMENT_RELRO_END(24, .);

  .got.plt : {
    *(.got.plt)
    *(.igot.plt)
  }

  .data : {
    __data_start = .;
    KEEP(*(SORT_BY_NAME(.piro.data.sort.*)))
    *(.data .data.* .gnu.linkonce.d.*)
    KEEP(*(SORT_BY_NAME(.sort.data.*)))
    SORT(CONSTRUCTORS)
  }

  _edata = .;
  PROVIDE(edata = .);

  . = .;
  __bss_start = .;
  __bss_start__ = .;

  .bss : {
    *(.dynbss)
    *(.bss .bss.* .gnu.linkonce.b.*)
    KEEP(*(SORT_BY_NAME(.piro.bss.sort.*)))
    *(COMMON)
    . = ALIGN(CONSTANT(COMMONPAGESIZE));
  }

  _bss_end__ = .;
  __bss_end__ = .;
  . = ALIGN(64 / 8);
  . = SEGMENT_START("ldata-segment", .);
  . = ALIGN(64 / 8);
  __end__ = .;
  _end = .;
  PROVIDE(end = .);

  . = DATA_SEGMENT_END(.);

  .stab               0 : { *(.stab) }
  .stabstr            0 : { *(.stabstr) }
  .stab.excl          0 : { *(.stab.excl) }
  .stab.exclstr       0 : { *(.stab.exclstr) }
  .stab.index         0 : { *(.stab.index) }
  .stab.indexstr      0 : { *(.stab.indexstr) }
  .gnu.build.attributes : { *(.gnu.build.attributes .gnu.build.attributes.*) }
  .debug              0 : { *(.debug) }
  .line               0 : { *(.line) }
  .debug_srcinfo      0 : { *(.debug_srcinfo) }
  .debug_sfnames      0 : { *(.debug_sfnames) }
  .debug_aranges      0 : { *(.debug_aranges) }
  .debug_pubnames     0 : { *(.debug_pubnames) }
  .debug_info         0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev       0 : { *(.debug_abbrev) }
  .debug_line         0 : { *(.debug_line .debug_line.* .debug_line_end) }
  .debug_frame        0 : { *(.debug_frame) }
  .debug_str          0 : { *(.debug_str) }
  .debug_loc          0 : { *(.debug_loc) }
  .debug_macinfo      0 : { *(.debug_macinfo) }
  .debug_weaknames    0 : { *(.debug_weaknames) }
  .debug_funcnames    0 : { *(.debug_funcnames) }
  .debug_typenames    0 : { *(.debug_typenames) }
  .debug_varnames     0 : { *(.debug_varnames) }
  .debug_pubtypes     0 : { *(.debug_pubtypes) }
  .debug_ranges       0 : { *(.debug_ranges) }
  .debug_macro        0 : { *(.debug_macro) }
  .debug_addr         0 : { *(.debug_addr) }
  .ARM.attributes     0 : { KEEP(*(.ARM.attributes)) KEEP(*(.gnu.attributes)) }
  .note.gnu.arm.ident 0 : { KEEP(*(.note.gnu.arm.ident)) }

  .zip 0 : {
    KEEP(*(.zip.file))
    __zip_cdir_start = .;
    KEEP(*(.zip.cdir))
    __zip_cdir_size = . - __zip_cdir_start;
    KEEP(*(.zip.eocd))
  }

  /DISCARD/ : {
    *(__patchable_function_entries)
    *(.GCC.command.line)
    *(.note.GNU-stack)
    *(.gnu_debuglink)
    *(.text.windows)
    *(.gnu.lto_*)
    *(.eh_frame)
    *(.idata.*)
    *(.yoink)
    *(.head)
  }
}

ape_stack_vaddr = DEFINED(ape_stack_vaddr) ? ape_stack_vaddr : 0x700000000000;
ape_stack_memsz = DEFINED(ape_stack_memsz) ? ape_stack_memsz : 8 * 1024 * 1024;
ape_stack_align = DEFINED(ape_stack_align) ? ape_stack_align : 16;
ape_stack_round = -ape_stack_align;

_tls_size = _tbss_end - _tdata_start;
_tdata_size = _tdata_end - _tdata_start;
_tbss_size = _tbss_end - _tbss_start;
_tbss_offset = _tbss_start - _tdata_start;
_tls_content = (_tdata_end - _tdata_start) + (_tbss_end - _tbss_start);
_tls_align = MAX(ALIGNOF(.tdata), ALIGNOF(.tbss));

ASSERT(ALIGNOF(.tdata) <= TLS_ALIGNMENT && ALIGNOF(.tbss) <= TLS_ALIGNMENT,
       "_Thread_local _Alignof can't exceed TLS_ALIGNMENT");
