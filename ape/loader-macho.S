/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
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

//	APE Loader Executable Structure for XNU

	.section .head

	.balign	64
macho:	.long	0xFEEDFACE+1
	.long	MAC_CPU_NEXGEN32E
	.long	MAC_CPU_NEXGEN32E_ALL
	.long	MAC_EXECUTE
	.long	4				// number of load commands
	.long	50f-10f				// size of all load commands
	.long	MAC_NOUNDEFS			// flags
	.long	0				// reserved
10:	.long	MAC_LC_SEGMENT_64
	.long	20f-10b				// unmaps first page dir
	.ascin	"__PAGEZERO",16			// consistent with linux
	.quad	0,0x200000,0,0			// which forbids mem <2m
	.long	0,0,0,0
20:	.long	MAC_LC_SEGMENT_64
	.long	30f-20b
	.ascin	"__TEXT",16
	.quad	macho				// vaddr
	.quad	filesz				// memsz
	.quad	0				// file offset
	.quad	filesz				// file size
	.long	PROT_EXEC|PROT_READ|PROT_WRITE	// maxprot
	.long	PROT_EXEC|PROT_READ		// initprot
	.long	0				// segment section count
	.long	0				// flags
30:	.long	MAC_LC_UUID
	.long	40f-30b
	.quad	0x3fb29ee4ac6c87aa		// uuid1
	.quad	0xdd2c9bb866d9eef8		// uuid2
40:	.long	MAC_LC_UNIXTHREAD
	.long	50f-40b				// cmdsize
	.long	MAC_THREAD_NEXGEN32E		// flavaflav
	.long	(420f-410f)/4			// count
410:	.quad	0				// rax
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
	.quad	_start				// rip
	.quad	0				// rflags
	.quad	0				// cs
	.quad	0				// fs
	.quad	0				// gs
420:
50:
	.endobj	macho,globl

	.balign	64
_start:	mov	$_HOSTXNU,%dl			// xnu's not unix!
	mov	%rsp,%rsi			// save real stack
	sub	$1024*1024,%rsp			// room for allocs
	jmp	ApeLoader
	.endfn	_start,globl

//	Invokes system call.
//
//	This function has eight parameters. The first seven are for
//	arguments passed along to the system call. The eight is for
//	the magic number that indicates which system call is called
//
//	The return value follows the Linux kernel convention, where
//	errors are returned as `-errno`. BSD systems are normalized
//	to follow this convention automatically.
SystemCall:
	mov	%rcx,%r10
	mov	16(%rsp),%eax
	clc
	syscall
	jnc	1f
	neg	%rax
1:	ret
	.endfn	SystemCall,globl
