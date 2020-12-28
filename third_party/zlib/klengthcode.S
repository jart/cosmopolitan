/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
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
#include "third_party/zlib/zutil.h"
#include "libc/macros.h"

	.initbss 300,_init_kZlibLengthCode
kZlibLengthCode:
	.zero	MAX_MATCH - MIN_MATCH + 1
	.endobj	kZlibLengthCode,globl,hidden
	.previous

	.initro 300,_init_kZlibLengthCode
.LkZlibLengthCode.rodata:	/* 64 bytes (25%) */
	.byte	1,0x00		/* 00-00 ∅   */
	.byte	1,0x01		/* 01-01 ☺   */
	.byte	1,0x02		/* 02-02 ☻   */
	.byte	1,0x03		/* 03-03 ♥   */
	.byte	1,0x04		/* 04-04 ♦   */
	.byte	1,0x05		/* 05-05 ♣   */
	.byte	1,0x06		/* 06-06 ♠   */
	.byte	1,0x07		/* 07-07 •   */
	.byte	2,0x08		/* 08-09 ◘-○ */
	.byte	2,0x09		/* 0a-0b ◙-♂ */
	.byte	2,0x0a		/* 0c-0d ♀-♪ */
	.byte	2,0x0b		/* 0e-0f ♫-☼ */
	.byte	4,0x0c		/* 10-13 ►-‼ */
	.byte	4,0x0d		/* 14-17 ¶-↨ */
	.byte	4,0x0e		/* 18-1b ↑-← */
	.byte	4,0x0f		/* 1c-1f ∟-▼ */
	.byte	8,0x10		/* 20-27  -' */
	.byte	8,0x11		/* 28-2f (-/ */
	.byte	8,0x12		/* 30-37 0-7 */
	.byte	8,0x13		/* 38-3f 8-? */
	.byte	16,0x14		/* 40-4f @-O */
	.byte	16,0x15		/* 50-5f P-_ */
	.byte	16,0x16		/* 60-6f `-o */
	.byte	16,0x17		/* 70-7f p-⌂ */
	.byte	32,0x18		/* 80-9f Ç-ƒ */
	.byte	32,0x19		/* a0-bf á-┐ */
	.byte	32,0x1a		/* c0-df └-▀ */
	.byte	31,0x1b		/* e0-fe α-■ */
	.byte	1,0x1c		/* ff-ff λ   */
	.endobj	.LkZlibLengthCode.rodata
	.byte	0,0		/* terminator */
	.byte	0,0,0,0		/* padding */
	.previous

	.init.start 300,_init_kZlibLengthCode
	call	rldecode
	lodsl
	.init.end 300,_init_kZlibLengthCode
