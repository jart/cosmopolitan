/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
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
#include "ape/relocations.h"
#include "libc/zip.h"
#include "libc/macros.h"

/	ZIP Central Directory.
	.section .piro.data.sort.zip.3,"a",@progbits
	.hidden	__zip_start
	.globl	__zip_start
	.type	__zip_start,@object
	.align	kZipCdirAlign
__zip_start:
	.previous/*
	...
	decentralized content
	...
	*/.section .piro.data.sort.zip.5,"a",@progbits
	.align	kZipCdirAlign
__zip_end:
	.long	kZipCdirHdrMagic	# magic
	.short	0			# disk
	.short	0			# starting disk
	.short	v_zip_records		# records on disk
	.short	v_zip_records		# records
	.long	v_zip_cdirsize		# size of central directory
	.long	RVA(__zip_start)	# central directory offset
	.short	v_zip_commentsize	# comment size
	.endobj	__zip_end,globl,hidden
	.weak	v_zip_records
	.weak	v_zip_cdirsize
	.weak	v_zip_commentsize
	.previous

	.yoink	__zipos_close
	.yoink	__zipos_fstat
	.yoink	__zipos_open
	.yoink	__zipos_parseuri
	.yoink	__zipos_read
	.yoink	__zipos_stat
	.source	__FILE__
