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
	.word	3			# e_phnum
	.word	0			# e_shentsize
	.word	0			# e_shnum
	.word	0			# e_shstrndx
	.endobj	ehdr,globl

//	memcpy(0x200000, loader); xor %eax,%eax; jmp 0x200000
jg47h:	.org	0x47
	.endobj	jg47h

_start:	mov	%rsp,%rsi
	jmp	loader
	.endfn	_start,globl

	.align	8
phdrs:	.long	PT_LOAD			# p_type
	.long	PF_R|PF_X		# p_flags
	.quad	0			# p_offset
	.quad	ehdr			# p_vaddr
	.quad	ehdr			# p_paddr
	.quad	filesz			# p_filesz
	.quad	filesz			# p_memsz
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
