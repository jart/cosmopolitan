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

	.align	8
ehdr:	.ascii	"\177ELF"
	.byte	ELFCLASS64
	.byte	ELFDATA2LSB
	.byte	1
	.byte	ELFOSABI_FREEBSD
	.quad	0
	.word	ET_EXEC			# e_type
	.word	EM_NEXGEN32E		# e_machine
	.long	1			# e_version
	.quad	_start			# e_entry
	.quad	phdrs - ehdr		# e_phoff
	.quad	0			# e_shoff
	.long	0			# e_flags
	.word	64			# e_ehsize
	.word	56			# e_phentsize
	.word	4			# e_phnum
	.word	0			# e_shentsize
	.word	0			# e_shnum
	.word	0			# e_shstrndx
	.endobj	ehdr,globl

	.align	8
phdrs:	.long	PT_LOAD			# p_type
	.long	PF_R|PF_X		# p_flags
	.quad	0			# p_offset
	.quad	ehdr			# p_vaddr
	.quad	ehdr			# p_paddr
	.quad	filesz			# p_filesz
	.quad	filesz			# p_memsz
	.quad	PAGESIZE		# p_align

	.long	PT_LOAD			# p_type
	.long	PF_R|PF_W		# p_flags
	.quad	0			# p_offset
	.quad	bss			# p_vaddr
	.quad	bss			# p_paddr
	.quad	0			# p_filesz
	.quad	bsssize			# p_memsz
	.quad	PAGESIZE		# p_align

	.long	PT_GNU_STACK		# p_type
	.long	PF_R|PF_W		# p_flags
	.quad	0			# p_offset
	.quad	0			# p_vaddr
	.quad	0			# p_paddr
	.quad	0			# p_filesz
	.quad	0			# p_memsz
	.quad	16			# p_align

	.long	PT_NOTE			# p_type
	.long	PF_R			# p_flags
	.quad	note - ehdr		# p_offset
	.quad	note			# p_vaddr
	.quad	note			# p_paddr
	.quad	notesize		# p_filesz
	.quad	notesize		# p_memsz
	.quad	8			# p_align
	.endobj	phdrs

note:	.long	2f-1f
	.long	4f-3f
	.long	1
1:	.asciz	"OpenBSD"
2:	.align	4
3:	.long	0
4:	.long	2f-1f
	.long	4f-3f
	.long	1
1:	.asciz	"NetBSD"
2:	.align	4
3:	.long	901000000
4:	.endobj	note
	notesize = . - note

	.align	64,0				# for ape.S dd
	.org	0x180				# for ape.S dd

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
	.long	5				# number of load commands
	.long	60f-10f				# size of all load commands
	.long	MAC_NOUNDEFS			# flags
	.long	0				# reserved
10:	.long	MAC_LC_SEGMENT_64
	.long	20f-10b				# unmaps first page dir
	.ascin	"__PAGEZERO",16			# consistent with linux
	.quad	0,0x200000,0,0			# which forbids mem <2m
	.long	0,0,0,0
20:	.long	MAC_LC_SEGMENT_64
	.long	30f-20b
	.ascin	"__TEXT",16
	.quad	ehdr				# vaddr
	.quad	4096				# memsz
	.quad	0				# file offset
	.quad	filesz				# file size
	.long	PROT_EXEC|PROT_READ|PROT_WRITE	# maxprot
	.long	PROT_EXEC|PROT_READ		# initprot
	.long	1				# segment section count
	.long	0				# flags
210:	.ascin	"__text",16			# section name (.text)
	.ascin	"__TEXT",16
	.quad	_start				# vaddr
	.quad	textsz				# memsz
	.long	textoff				# offset
	.long	6				# align 2**6 = 64
	.long	0				# reloc table offset
	.long	0				# relocation count
	.long	MAC_S_ATTR_SOME_INSTRUCTIONS	# section type & attributes
	.long	0,0,0				# reserved
30:	.long	MAC_LC_SEGMENT_64
	.long	40f-30b
	.ascin	"__DATA",16
	.quad	bss				# vaddr
	.quad	bsssize				# memsz
	.quad	0				# offset
	.quad	0				# file size
	.long	PROT_EXEC|PROT_READ|PROT_WRITE	# maxprot
	.long	PROT_READ|PROT_WRITE		# initprot
	.long	1				# segment section count
	.long	0				# flags
310:	.ascin	"__bss",16			# section name (.bss)
	.ascin	"__DATA",16
	.quad	bss				# vaddr
	.quad	bsssize				# memsz
	.long	0				# offset
	.long	12				# align 2**12 = 4096
	.long	0				# reloc table offset
	.long	0				# relocation count
	.long	MAC_S_ZEROFILL			# section type & attributes
	.long	0,0,0				# reserved
40:	.long	MAC_LC_UUID
	.long	50f-40b
	.quad	0x3fb29ee4ac6c87aa		# uuid1
	.quad	0xdd2c9bb866d9eef8		# uuid2
50:	.long	MAC_LC_UNIXTHREAD
	.long	60f-50b				# cmdsize
	.long	MAC_THREAD_NEXGEN32E		# flavaflav
	.long	(520f-510f)/4			# count
510:	.quad	0				# rax
	.quad	0				# rbx
	.quad	0				# rcx
	.quad	_HOSTXNU			# rdx
	.quad	0				# rdi
	.quad	0				# rsi
	.quad	0				# rbp
	.quad	0				# rsp
	.quad	0				# r8
	.quad	0				# r9
	.quad	0				# r10
	.quad	0				# r11
	.quad	0				# r12
	.quad	0				# r13
	.quad	0				# r14
	.quad	0				# r15
	.quad	_start				# rip
	.quad	0				# rflags
	.quad	0				# cs
	.quad	0				# fs
	.quad	0				# gs
520:
60:
	.endobj	macho

	.align	64,0				# for ape.S dd
	.org	0x400				# for ape.S dd

//	Ape Loader Entrpoint
//
//	This is normally called by the operating system. However it may
//	be called by the Actually Portable Executables themselves, when
//	re-executing a program. Just do this:
//
//	    memcpy(0x200000, loader)
//	    lea handoff(%rip),%rcx
//	    lea argblock(%rip),%rsp
//	    jmp 0x200400
//
//	@see	APE_LOADER_ENTRY
//	@see	ape/loader.h
_start:

//	Hack for detecting M1 Rosetta environment.
//	https://github.com/jart/cosmopolitan/issues/429#issuecomment-1166704377
	cmp	$-1,%ebx
	jne	0f
	cmp	$+1,%edx
	jne	0f
	mov	$_HOSTXNU,%dl
	xor	%ecx,%ecx

0:	mov	%rsp,%rsi
	jmp	ApeLoader
	.endfn	_start,globl

//	System Call Entrpoint
//
//	This function is used by the APE loader to make system calls.
//	We also pass a reference to this function to the APE binary's
//	_start() function. It's needed because on OpenBSD, msyscall()
//	restricts which pages can issue system calls, and it can only
//	be called once. Therefore if we want to be load and re-load a
//	binary multiple times without calling the system execve(), we
//	need to be able to handover the SYSCALL function. We hardcode
//	this to a fixed address, but that shouldn't be used, since we
//	would ideally want to move it to a random page in the future.
__syscall_loader:
	clc
	syscall
	jc	1f
	ret
1:	neg	%rax
	ret
	.endfn	__syscall_loader,globl
