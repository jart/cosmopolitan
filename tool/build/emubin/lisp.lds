/*-*- mode: ld-script; indent-tabs-mode: nil; tab-width: 2; coding: utf-8   -*-│
│vi: set et sts=2 tw=2 fenc=utf-8                                           :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/

ENTRY(_start)

SECTIONS {

  .text 0x7c00 - 0x600 : {
    *(.start .start.*)
    rodata = .;
    *(.rodata .rodata.*)
    . = 0x1fe;
    SHORT(0xaa55);
    *(.text .text.*)
    /*BYTE(0x90);*/
    _etext = .;
    . = ALIGN(512);
  }

  .bss : {
    bss = .;
    *(.bss .bss.*)
    *(COMMON)
  }

  /DISCARD/ : {
    *(.*)
  }
}

boot        = 0x7c00;
q.syntax    = 8192*2;
q.look      = 8192*2+256;
q.globals   = 8192*2+256+2;
q.index     = 8192*2+256+2+2;
q.token     = 8192*2+256+2+2+2;
q.str       = 8192*2+256+2+2+2+128;
v_sectors   = SIZEOF(.text) / 512;
