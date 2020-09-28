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
#include "libc/runtime/runtime.h"
#include "libc/macros.h"

#define kTmpPathMax 80

/	RII constant holding /tmp/ directory.
/
/	@note	on win32 this is firstNonNull($TMP, $TEMP, $PWD)
/	@note	guarantees trailing slash if non-empty
	.initbss 300,_init_kTmpPath
kTmpPath:
	.zero	kTmpPathMax
	.endobj	kTmpPath,globl
	.previous

	.init.start 300,_init_kTmpPath
	movl	$'/|'t<<010|'m<<020|'p<<030,(%rdi)
	movw	$'/,4(%rdi)
	pushpop	kTmpPathMax,%rdx
	ezlea	GetTempPathA$flunk,ax
	call	__getntsyspath
	.init.end 300,_init_kTmpPath
	.source	__FILE__
