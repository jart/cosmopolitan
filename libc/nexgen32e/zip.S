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
#include "libc/macros.internal.h"
#include "ape/relocations.h"
#include "libc/zip.h"

//	ZIP Central Directory.
	.section .zip.3,"",@progbits
	.hidden	__zip_start
	.globl	__zip_start
	.type	__zip_start,@object
__zip_start:
	.previous/*
	...
	decentralized content
	...
	*/.section .zip.5,"",@progbits
__zip_end:
	.long	kZipCdirHdrMagic	# magic
	.short	0			# disk
	.short	0			# starting disk
	.short	v_zip_records		# number of records on disk
	.short	v_zip_records		# records
	.long	v_zip_cdirsize		# size of central directory
	.long	RVA(__zip_start)	# central directory offset
	.short	v_zip_commentsize	# comment size
	.endobj	__zip_end,globl,hidden
	.weak	v_zip_records
	.weak	v_zip_cdirsize
	.weak	v_zip_commentsize
	.previous
