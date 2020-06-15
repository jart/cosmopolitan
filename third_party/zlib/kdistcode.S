/*-*- mode:asm; indent-tabs-mode:t; tab-width:8; coding:utf-8               -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
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
#include "third_party/zlib/deflate.h"
#include "libc/macros.h"

	.initbss 300,_init_kZlibDistCode
kZlibDistCode:
	.zero	DIST_CODE_LEN
	.endobj	kZlibDistCode,globl,hidden
	.previous

	.initro 300,_init_kZlibDistCode
.LkZlibDistCode.rodata:		/* 64 bytes (13%) */
	.byte	1,0x00		/* 00-00 ∅   */
	.byte	1,0x01		/* 01-01 ☺   */
	.byte	1,0x02		/* 02-02 ☻   */
	.byte	1,0x03		/* 03-03 ♥   */
	.byte	2,0x04		/* 04-05 ♦-♣ */
	.byte	2,0x05		/* 06-07 ♠-• */
	.byte	4,0x06		/* 08-0b ◘-♂ */
	.byte	4,0x07		/* 0c-0f ♀-☼ */
	.byte	8,0x08		/* 10-17 ►-↨ */
	.byte	8,0x09		/* 18-1f ↑-▼ */
	.byte	16,0x0a		/* 20-2f  -/ */
	.byte	16,0x0b		/* 30-3f 0-? */
	.byte	32,0x0c		/* 40-5f @-_ */
	.byte	32,0x0d		/* 60-7f `-⌂ */
	.byte	64,0x0e		/* 80-bf Ç-┐ */
	.byte	64,0x0f		/* c0-ff └-λ */
	.byte	2,0x00		/* 100-101   */
	.byte	1,0x10		/* 102-102   */
	.byte	1,0x11		/* 103-103   */
	.byte	2,0x12		/* 104-105   */
	.byte	2,0x13		/* 106-107   */
	.byte	4,0x14		/* 108-10b   */
	.byte	4,0x15		/* 10c-10f   */
	.byte	8,0x16		/* 110-117   */
	.byte	8,0x17		/* 118-11f   */
	.byte	16,0x18		/* 120-12f   */
	.byte	16,0x19		/* 130-13f   */
	.byte	32,0x1a		/* 140-15f   */
	.byte	32,0x1b		/* 160-17f   */
	.byte	64,0x1c		/* 180-1bf   */
	.byte	64,0x1d		/* 1c0-1ff   */
	.endobj	.LkZlibDistCode.rodata
	.byte	0,0	/* terminator */
	.previous

	.init.start 300,_init_kZlibDistCode
	call	rldecode
	.init.end 300,_init_kZlibDistCode
