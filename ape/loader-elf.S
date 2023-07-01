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
#include "libc/elf/def.h"
#include "libc/sysv/consts/prot.h"
#include "libc/macho.internal.h"
#include "libc/dce.h"
#include "libc/macros.internal.h"

//	APE Loader Executable Structure
//	Linux, FreeBSD, NetBSD, OpenBSD

	.section .head

	.balign	8
ehdr:	.ascii	"\177ELF"
	.byte	ELFCLASS64
	.byte	ELFDATA2LSB
	.byte	1
	.byte	ELFOSABI_FREEBSD
	.quad	0
	.word	ET_EXEC			// e_type
	.word	EM_NEXGEN32E		// e_machine
	.long	1			// e_version
	.quad	_start			// e_entry
	.quad	phdrs - ehdr		// e_phoff
	.quad	0			// e_shoff
	.long	0			// e_flags
	.word	64			// e_ehsize
	.word	56			// e_phentsize
	.word	4			// e_phnum
	.word	0			// e_shentsize
	.word	0			// e_shnum
	.word	0			// e_shstrndx
	.endobj	ehdr,globl

	.balign	8
phdrs:	.long	PT_LOAD			// p_type
	.long	PF_R			// p_flags
	.quad	0			// p_offset
	.quad	ehdr			// p_vaddr
	.quad	ehdr			// p_paddr
	.quad	rosize			// p_filesz
	.quad	rosize			// p_memsz
	.quad	4096			// p_align

	.long	PT_LOAD			// p_type
	.long	PF_X			// p_flags
	.quad	rosize			// p_offset
	.quad	text			// p_vaddr
	.quad	text			// p_paddr
	.quad	textsz			// p_filesz
	.quad	textsz			// p_memsz
	.quad	4096			// p_align

	.long	PT_GNU_STACK		// p_type
	.long	PF_R|PF_W		// p_flags
	.quad	0			// p_offset
	.quad	0			// p_vaddr
	.quad	0			// p_paddr
	.quad	0			// p_filesz
	.quad	8*1024*1024		// p_memsz
	.quad	16			// p_align

	.long	PT_NOTE			// p_type
	.long	PF_R			// p_flags
	.quad	note - ehdr		// p_offset
	.quad	note			// p_vaddr
	.quad	note			// p_paddr
	.quad	notesize		// p_filesz
	.quad	notesize		// p_memsz
	.quad	8			// p_align
	.endobj	phdrs

note:	.long	2f-1f
	.long	4f-3f
	.long	1
1:	.asciz	"OpenBSD"
2:	.balign	4
3:	.long	0
4:	.long	2f-1f
	.long	4f-3f
	.long	1
1:	.asciz	"NetBSD"
2:	.balign	4
3:	.long	901000000
4:	.endobj	note
	notesize = . - note

	.balign	64,0				// for ape.S dd
	.org	64*6				// for ape.S dd

//	APE Loader XNU Header
//
//	This header is dd'd backwards by the APE shell script when
//	running on Mac OS X.
//
//	@see	ape/ape.S
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
	.quad	ehdr				// vaddr
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
	.quad	0xdd2c9bb866d9eef9		// uuid2
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
	.quad	_apple				// rip
	.quad	0				// rflags
	.quad	0				// cs
	.quad	0				// fs
	.quad	0				// gs
420:
50:
	.endobj	macho

	.balign	64,0				// for ape.S dd
	.org	64*12				// for ape.S dd

	.text
_apple:	mov	$_HOSTXNU,%dl			// xnu's not unix!
_start:	mov	%rsp,%rsi			// save real stack
	sub	$1024*1024,%rsp			// room for allocs
	jmp	ApeLoader
	.endfn	_start,globl
	.endfn	_apple,globl

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
