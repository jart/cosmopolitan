/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set noet ft=asm ts=8 sw=8 fenc=utf-8                                 :vi │
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
#include "libc/macros.internal.h"
#include "ape/relocations.h"
#include "libc/zip.internal.h"
.section .zip.eocd,"",@progbits

//	ZIP End Of Central Directory (EOCD) record.
	.globl	__zip_start
__zip_start:
__zip_eocd:				// let symbol be discarded
	.long	kZipCdirHdrMagicTodo	// magic (scan backwards for this)
	.short	0			// disk
	.short	0			// starting disk
	.short	0			// number of records on disk
	.short	0			// records
	.endobj	__zip_eocd,globl

__zip_eocd.size:
	.weak	__zip_cdir_size
	.long	__zip_cdir_size
	.endobj	__zip_eocd.size

__zip_eocd.offset:
	.long	0
	.endobj	__zip_eocd.offset

__zip_eocd.commentsize:
	.short	0
	.endobj	__zip_eocd.commentsize
