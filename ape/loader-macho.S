/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set noet ft=asm ts=8 sw=8 fenc=utf-8                                 :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/macho.internal.h"
#include "libc/sysv/consts/prot.h"
#include "libc/dce.h"
#include "libc/macros.internal.h"

//	Apple Mach-O Executable Headers
//	Fixups are applied by objbincopy.com
//	There must exist a MAC_LC_SEGMENT_64 for every PT_LOAD

	.section .macho,"a",@progbits
	.balign	64
	.long	0xFEEDFACE+1
	.long	MAC_CPU_NEXGEN32E
	.long	MAC_CPU_NEXGEN32E_ALL
	.long	MAC_EXECUTE
	.long	5				// number of load commands
	.long	60f-10f				// size of all load commands
	.long	MAC_NOUNDEFS|MAC_SPLIT_SEGS	// flags
	.long	0				// reserved
10:	.long	MAC_LC_SEGMENT_64
	.long	20f-10b				// unmaps first page dir
	.ascin	"__PAGEZERO",16			// consistent with linux
	.quad	0,0x200000,0,0			// which forbids mem <2m
	.long	0,0,0,0
20:	.long	MAC_LC_SEGMENT_64
	.long	30f-20b
	.ascin	"__TEXT",16
	.quad	0				// vaddr
	.quad	0				// memsz
	.quad	0				// file offset
	.quad	0				// file size
	.long	0				// maxprot
	.long	0				// initprot
	.long	0				// segment section count
	.long	0				// flags
30:	.long	MAC_LC_SEGMENT_64
	.long	40f-30b
	.ascin	"__RODATA",16
	.quad	0				// vaddr
	.quad	0				// memsz
	.quad	0				// file offset
	.quad	0				// file size
	.long	0				// maxprot
	.long	0				// initprot
	.long	0				// segment section count
	.long	0				// flags
40:	.long	MAC_LC_UUID
	.long	50f-40b
	.quad	0x4527148ba7a513ef		// uuid1
	.quad	0x56fa865940665e8f		// uuid2
50:	.long	MAC_LC_UNIXTHREAD
	.long	60f-50b				// cmdsize
	.long	MAC_THREAD_NEXGEN32E		// flavaflav
	.long	(520f-510f)/4			// count
510:	.quad	0				// rax
	.quad	0				// rbx
	.quad	0				// rcx
	.quad	0				// rdx
	.quad	0				// rdi
	.quad	0				// rsi
	.quad	0				// rbp
	.quad	0				// rsp
	.quad	0				// r8
	.quad	0				// r9
	.quad	0				// r10
	.quad	0				// r11
	.quad	0				// r12
	.quad	0				// r13
	.quad	0				// r14
	.quad	0				// r15
	.quad	XnuEntrypoint			// rip
	.quad	0				// rflags
	.quad	0				// cs
	.quad	0				// fs
	.quad	0				// gs
520:
60:
