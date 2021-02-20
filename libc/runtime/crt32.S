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
#include "ape/macros.internal.h"
#include "libc/dce.h"
#include "libc/elf/def.h"
#include "libc/macros.h"
.section .start,"ax",@progbits
.code32

//	@fileoverview 80386 APE UNIX CRT
//
//	This module is yoinked by build/i386ify.sh when GNU make
//	builds source codes that have the .i386.c file extension
//	thereby avoiding some 386 bloat when it isn't being used
//
//	TODO: Verify we can't use 64-bit ELF with i386 e_machine

_start32:
	.weak	main32
	call	main32
	mov	%eax,%ebx
	pushpop	1,%eax				# _exit() ordinal
	push	%ebx				# hybrid calling convention
	push	%eax				# should work on Linux+BSDs
	int	$0x80
	ud2
	.endfn	_start32,globl,hidden

	.section .elf.phdrs32,"a",@progbits
	.long	PT_LOAD				# text segment
	.stub	ape_rom_offset,long
	.stub	ape_rom_vaddr,long
	.stub	ape_rom_paddr,long
	.stub	ape_rom_filesz,long
	.stub	ape_rom_memsz,long
	.long	PF_R|PF_X			# <-- wut
	.stub	ape_rom_align,long
	.long	PT_LOAD				# data segment
	.stub	ape_ram_offset,long
	.stub	ape_ram_vaddr,long
	.stub	ape_ram_paddr,long
	.stub	ape_ram_filesz,long
	.stub	ape_ram_memsz,long
	.long	PF_R|PF_W
	.stub	ape_ram_align,long
#if SupportsLinux()
//	Linux ignores mprotect() and returns 0 without this lool
//	It has nothing to do with the stack, which is still exec
	.long	PT_GNU_STACK			# p_type
	.long	0				# p_offset
	.long	0				# p_vaddr
	.long	0				# p_paddr
	.long	0				# p_filesz
	.long	0				# p_memsz
	.long	PF_R|PF_W			# p_flags
	.long	16				# p_align
#endif
#if SupportsOpenbsd() || SupportsNetbsd()
	.long	PT_NOTE				# notes
	.stub	ape_note_offset,long
	.stub	ape_note_vaddr,long
	.stub	ape_note_paddr,long
	.stub	ape_note_filesz,long
	.stub	ape_note_memsz,long
	.long	PF_R
	.stub	ape_note_align,long
#endif
	.previous
