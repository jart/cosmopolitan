/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set noet ft=asm ts=8 sw=8 fenc=utf-8                                 :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice & this permission notice appear in all copies         │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS.  IN NO EVENT SHALL                │
│ THE AUTHOR BE LIABLE FOR ANY SPECIAL DIRECT INDIRECT, OR EVEN                │
│ CONSEQUENTIAL DAMAGE OR ANY DAMAGES WHATSOEVER RESULTING FROM                │
│ LOSS OF USE, DATA OR PROFITS WHETHER IN AN ACTION OF CONTRACT                │
│ NEGLIGENCE OR OTHER TORTIOUS ACTION, THAT ARISE OUT OF, OR IN                │
│ CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.                     │
╠──────────────────────────────────────────────────────────────────────────────╣
│░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░█▀█░█▀█░▀█▀░█░█░█▀█░█░░░█░░░█░█░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░█▀█░█░▄░░█░░█░█░█▀█░█░░░█░░░▀█▀░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░▀░▀░▀▀▀░░▀░░▀▀▀░▀░▀░▀▀▀░▀▀▀░░▀░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░█▀█░█▀█░█▀█░▀█▀░█▀█░█▀█░█░░░█▀▀░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░█▀▀░█ █░██▀░░█░░█▀█░█▀█░█░░░█▀▀░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░▀░░░▀▀▀░▀░▀░░▀░░▀░▀░▀▀▀░▀▀▀░▀▀▀░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░█▀▀░█░█░█▀▀░█▀█░█░█░▀█▀░█▀█░█▀█░█░░█▀▀░░░░░░░░░░░░░░░░░░░░░░░░▄▄░░░▐█░░│
│░░░░░░░█▀▀░▄▀▄░█▀▀░█░▄░█░█░░█░░█▀█░█▀█░█░░█▀▀░░░░░░░░░░░░▄▄▄░░░▄██▄░░█▀░░░█░▄░│
│░░░░░░░▀▀▀░▀░▀░▀▀▀░▀▀▀░▀▀▀░░▀░░▀░▀░▀▀▀░▀▀░▀▀▀░░░░░░░░░░▄██▀█▌░██▄▄░░▐█▀▄░▐█▀░░│
│░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░▐█▀▀▌░░░▄▀▌░▌░█░▌░░▌░▌░░│
╠──────────────────────────────────────────────────────▌▀▄─▐──▀▄─▐▄─▐▄▐▄─▐▄─▐▄─│
│ αcτµαlly pδrταblε εxεcµταblε § program header                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "ape/macros.internal.h"
#include "ape/notice.inc"
#include "ape/relocations.h"
#include "libc/calls/metalfile.internal.h"
#include "libc/dce.h"
#include "libc/elf/def.h"
#include "libc/macho.internal.h"
#include "libc/nexgen32e/uart.internal.h"
#include "libc/nt/pedef.internal.h"
#include "libc/runtime/pc.internal.h"
#include "ape/ape.internal.h"
#include "libc/thread/tls.h"
#include "ape/ape.h"
#include "libc/sysv/consts/prot.h"

#define USE_SYMBOL_HACK 1

	.section .text,"ax",@progbits
	.balign	__SIZEOF_POINTER__
	.previous
	.section .rodata,"a",@progbits
	.balign	__SIZEOF_POINTER__
__ro:	.endobj	__ro,globl,hidden		// ←for gdb readability
	.previous
	.section .data,"aw",@progbits
	.balign	__SIZEOF_POINTER__
	.previous
	.section .bss,"aw",@nobits
	.balign	__SIZEOF_POINTER__
	.previous
	.section .rodata.str1.1,"a",@progbits
cstr:	.endobj	cstr,globl,hidden		// ←for gdb readability
	.previous
	.section .head,"ax",@progbits

/*	    ████████               ████████        ███████████
	    ██░░░░▒▒██           ██░░░░▒▒██    ████░░░░░░░░░▒▒████
	    ██░░░░░░██           ██░░░░▒▒██  ██░░░░▒▒███████░░░░▒▒██
	    ██░░░░░░▒▒██      ░██░░░░░░▒▒██  ██░░▒▒██       ██░░▒▒██
	    ██░░▒▒░░░░██      ░██░░░░░░▒▒██  ██░░▒▒██       ████████
	    ██░░▒▒▒▒░░▒▒██  ██▓░░░░▒▒░░▒▒██  ██░░▒▒▒▒███████
	    ██░░▒▒██░░░░██  ██▓░░░░██░░▒▒██    ████░░░░░░░▒▒████
	    ██░░▒▒████░░▒▒██░░░░░████░░▒▒██        ███████░░░░▒▒██
	    ██░░▒▒████░░░░██░░░░░████░░▒▒██  ██████       ██░░░░▒▒██
	    ██░░▒▒██  ██░░▒▒░░▒██  ██░░▒▒██  ██░░░░██       ██░░▒▒██
	    ██░░▒▒██  ██░░░░░░▒██  ██░░▒▒██  ██░░░░█████████░░░░▒▒██
	    ██░░▒▒██  ▓▓▒▒░░▒▒▒▓▓  ██░░▒▒██  ▓▓▒▒░░▓▓▓▓▓▓▓▓▓░░░░▓▓▓▓
	    ██░░▒▒██    ██░░██▓    ██░░▒▒██    ██░░░░░░░░░░░░░▒▒██
	  ████████████████████▓  ██████████████  ███████████████
	  ██▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓█████▓▓░░▓▓░░▓▓░░████░░░░░░░░░░░░░░░████
	  ██▓▓▓▓▓▓▓▓▓▓▓▓▓▓█████▓▓░░▓▓░░▓▓░░████░░░░░░░░░░░░░░░░░░░░░██
	  ██▓▓▓▓▓▓▓▓▓▓▓▓▓▓██▓▓▒░░▓▓░░▓▓░░▓▓██░░░░░░░░░░░░░░░░░░░░░░░░░██
	  ██▓▓▓▓▓▓██████████░░▒▓▓▓▓██████▓▓██░░░░░░███████████▓▓░░░░▓▓██
	  ██▓▓▓▓▓▓████████░░▓▓▓▓▓██▓▓▓▓██████░░░░████░░▒▓▓██  ██▓▓▓▓▓▓██
	  ██▓▓▓▓▓▓████████▓▓░░▒████▓▓▓▓██████░░░░██▓▓▓▓▒░░██    ██████
	  ██▓▓▓▓▓▓██    ██░░▓▓▓▓▓██▓▓▓▓██  ██▓▓░░████░░▒▓▓██████
	  ██▓▓▓▓▓▓██    ██▓▓░░▒████▓▓▓▓██    ██▓▓██▓▓▓▓▒░░██░░░░████
	  ██▓▓▓▓▓▓██    ██░░▓▓▓▓▓██▓▓▓▓██      ██████░░▒▓▓██░░░░░░░░██
	  ██▓▓▓▓▓▓██    ██▓▓░░▒████▓▓▓▓██        ██▓▓▓▓▒░░██▓▓░░░░░░░░██
	  ██▓▓▓▓▓▓██    ██░░▓▓▓████▓▓▓▓██    ████████░░▒▓▓████▓▓░░░░░░██
	  ██▓▓▓▓▓▓██      ██░░▒██▓▓▓▓████  ██░░░░░░██▓▓███    ██░░░░░░██
	  ██▓▓▓▓▓▓█████████████▓▓▓▓██████████▓▓░░░░░░█████████░░░░░░░░██
	  ██▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓████▓▓░░██▓▓░░░░░░░░░░░░░░░░░░░░░▓▓██
	  ██▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓████▓▓░░▓▓██▓▓▓▓░░░░░░░░░░░░░░░░░▓▓▓▓██
	  ██▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓████▓▓░░▓▓░░▓▓██▓▓▓▓▓▓░░░░░░░░░░░▓▓▓▓▓▓██
	  ██▓▓█████████████████████▓▓██▓▓██▓▓████▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓██
	  ███████████████████████████▓▓██▓▓██▓▓████▓▓▓▓▓▓▓▓▓▓▓▓▓████
	    ██████████████████▓    ██████████████  █████████████
╔──────────────────────────────────────────────────────────────────────────────╗
│ αcτµαlly pδrταblε εxεcµταblε § the old technology                            │
╚─────────────────────────────────────────────────────────────────────────────*/

#if SupportsWindows() || SupportsMetal() || SupportsXnu()

//	MZ Literally Executable Header
//
//	This is the beginning of the program file and it can serve as an
//	entrypoint too. It shouldn't matter if the program is running on
//	Linux, Windows, etc. Please note if the underlying machine isn't
//	a machine, this header may need to morph itself to say the magic
//	words, e.g. ⌂ELF, which also works fine as a generic entrypoint.
//
//	@param	dl is drive number
//	@noreturn
ape_mz:
#if SupportsWindows() || SupportsMetal()
	.asciz	"MZqFpD='\n"		// Mark 'Zibo' Joseph Zbikowski
//	# in real mode
//	dec	%bp
//	pop	%dx
//	jno	0x4a
//	jo	0x4a
//
//	# in legacy mode
//	push	%ebp
//	pop	%edx
//	jno	0x4a
//	jo	0x4a
//
//	# 64-bit mode
//	rex.WRB
//	pop	%r10
//	jno	0x4a
//	jo	0x4a
#else
//	Avoid virus scanner reputation damage when targeting System Five.
	.asciz	"jartsr='\n"		// Justine Alexandra Roberts Tunney
//	push	$0x61
//	jb	0x78
//	jae	0x78
#endif
	.short	0x1000			// MZ: lowers upper bound load / 16
	.short	0xf800			// MZ: roll greed on bss
	.short	0			// MZ: lower bound on stack segment
	.short	0			// MZ: initialize stack pointer
	.short	0			// MZ: ∑bₙ checksum don't bother
	.short	0x0100			// MZ: initial ip value
	.short	0x0800			// MZ: increases cs load lower bound
	.short	0x0040			// MZ: reloc table offset
	.short	0			// MZ: overlay number
	.org	0x24			// MZ: bytes reserved for you
	.ascii	"JT"			// MZ: OEM identifier
	.short	0			// MZ: OEM information
	.ascii	"' <<'@'\n"
	.org	0x40-4			// MZ: bytes reserved for you
#if SupportsWindows() || SupportsMetal()
	.long	RVA(ape_pe)		// PE: the new technology
#else
	.long	0
	.org	0x78
	pop	%rax
	jmp	_start
#endif
	.endfn	ape_mz,globl,hidden

#else /* !(SupportsWindows() || SupportsMetal() || SupportsXnu()) */

//	ELF Literally Executable Header
//
//	If we don't need to support Microsoft or Apple then we can
//	produce a conventional executable without the shell script
//
//	@param	dl is drive number
//	@noreturn
	.ascii	"\177ELF"			// 0x0: ⌂ELF
	.byte	ELFCLASS64			//   4: long mode
	.byte	ELFDATA2LSB			//   5: little endian
	.byte	1				//   6: elf v1.o
	.byte	ELFOSABI_FREEBSD		//   7: FreeBSD
	.byte	0				//   8: os/abi ver.
	.balign	8,0				//   9: padding
	.short	ET_EXEC				//  10: εxεcµταblε
#ifdef __x86_64__
	.short	EM_NEXGEN32E			//  12: amd
#elif __aarch64__
	.short	EM_AARCH64			//  12: arm
#elif __powerpc64__
	.short	EM_PPC64			//  12: open power
#elif __riscv
	.short	EM_RISCV			//  12: risc five
#endif
	.long	1				//  14: elf v1.o
	.quad	ape_elf_entry			//  18: e_entry
	.quad	ape_elf_phoff			//  20: e_phoff
	.quad	ape_elf_shoff			//  28: e_shoff
	.long	0				//  30: e_flags
	.short	64				//  34: e_ehsize
	.short	56				//  36: e_phentsize
	.short	ape_elf_phnum			//  38: e_phnum
	.short	0				//  3a: e_shentsize
	.short	ape_elf_shnum			//  3c: e_shnum
	.short	ape_elf_shstrndx		//  3e: e_shstrndx

#endif /* SupportsWindows() || SupportsMetal() || SupportsXnu() */

#if SupportsMetal()

//	Disk Operating System Stub
//
//	@param	dl is drive number
//	@noreturn
	.org	0x40			// mz/elf header length
stub:	mov	$0x40,%dl		// *literally* dos
	jmp	1f			// good bios skips here
1:	jmp	pc			// thus avoiding heroics
	nop				// system five bootpoint
	.org	0x48,0x90		// note ⌂ELF means JG 47
	jmp	3f			// MZ also means pop r10
2:	sub	$8,%rsp			// a.k.a. dec %ax sub %sp
	xor	%edx,%edx		// MZ ate BIOS drive code
3:	.byte	0xbd,0,0		// a.k.a. mov imm,%bp
	jmp	pc			// real mode, is real
	jmp	_start			// surprise it's unix
	.endfn	stub,globl

/*─────────────────────────────────────────────────────────────────────────────╗
│ αcτµαlly pδrταblε εxεcµταblε § ibm personal computer                         │
╚──────────────────────────────────────────────────────────────────────────────┘
	IBM designed BIOS to run programs by handing over the computer
	to a program as soon as its first sector is loaded. That gives
	us control over user-facing latency, even though the next step
	will generally be asking the BIOS to load more.

	The process is trivial enough that this entrypoint can support
	handoffs from alternative program-loaders e.g. Grub and MS-DOS
	so long as they either load our full program, or implement the
	PC BIOS disk service API.

	Since so many different implementations of these APIs have been
	built the last forty years these routines also canonicalize the
	cpu and program state, as it is written in the System V ABI. */

//	Initializes program and jumps to real mode loader.
//
//	@param	dl drive number (use 0x40 to skip bios disk load)
//	@mode	real
//	@noreturn
pc:	cld
	.code16
#if USE_SYMBOL_HACK
	.byte	0x0f,0x1f,0207			// nop rdi binbase
	.short	(0x7c00-IMAGE_BASE_VIRTUAL)/512
#endif
	mov	$0x70000>>4,%di			// we need a stack
	xor	%cx,%cx				// 0x7f000-0x80000
	mov	%cx,%es
	rlstack	%di,%cx
	push	%cs				// determine load address
	pop	%ds				// and relocate this code
	call	1f				// to a way lower address
1:	pop	%si				// and we'll make cs zero
	sub	$RVA(1b),%si
	mov	$IMAGE_BASE_REAL>>4,%ax
	push	%ax				// save IMAGE_BASE_REAL>>4
	push	%ax
	pop	%es
	xor	%di,%di
	mov	$512,%cx
	rep movsb
#if USE_SYMBOL_HACK
	.byte	0x0f,0x1f,0207			// nop rdi binbase
	.short	(IMAGE_BASE_REAL-0x7c00)/512
#endif
	ljmp	$0,$REAL(1f)
1:	mov	%cx,%ds
	.set	mm,0x0500			// struct mman
	mov	$IMAGE_BASE_REAL-mm,%cx		// clears bss
	mov	$mm>>4,%ax
	mov	%ax,%es
	xor	%ax,%ax
	xor	%di,%di
	rep stosb
	cmp	$0x40,%dl
	je	6f
	call	dsknfo
	pop	%es				// restore IMAGE_BASE_REAL>>4
	mov	$1,%al				// current sector
	xor	%cx,%cx				// current cylinder
	xor	%dh,%dh				// current head
	mov	$v_ape_realsectors,%di		// total sectors
3:	call	pcread
	mov	%es,%si				// addr += 512
	add	$512>>4,%si
	mov	%si,%es
	dec	%di
	jnz	3b
6:	ljmp	$0,$REAL(realmodeloader)
	.endfn	pc

//	Determines disk geometry.
//
//	We use imperial measurements for storage systems so the software
//	can have an understanding of physical locality, which deeply
//	impacts the latency of operations.
//
//	 - 160KB:  40 cylinders × 1 head  ×  8 sectors × 512 =   163,840
//	 - 180KB:  40 cylinders × 1 head  ×  9 sectors × 512 =   184,320
//	 - 320KB:  40 cylinders × 2 heads ×  8 sectors × 512 =   327,680
//	 - 360KB:  40 cylinders × 2 heads ×  9 sectors × 512 =   368,640
//	 - 720KB:  80 cylinders × 2 heads ×  9 sectors × 512 =   737,280
//	 - 1.2MB:  80 cylinders × 2 heads × 15 sectors × 512 = 1,228,800
//	 - 1.44MB: 80 cylinders × 2 heads × 18 sectors × 512 = 1,474,560
//
//	Terminology
//
//	 - Heads are also known as Tracks
//
//	Disk Base Table
//
//	   0: specify byte 1, step-rate time, head unload time
//	   1: specify byte 2, head load time, DMA mode
//	   2: timer ticks to wait before disk motor shutoff
//	   3: bytes per sector code
//	        0: 128 bytes  2: 512 bytes
//	        1: 256 bytes  3: 1024 bytes
//	   4: sectors per track (last sector number)
//	   5: inter-block gap length/gap between sectors
//	   6: data length, if sector length not specified
//	   7: gap length between sectors for format
//	   8: fill byte for formatted sectors
//	   9: head settle time in milliseconds
//	  10: motor startup time in eighths of a second
//
//	@param	dl drive number
//	@return	dl = pc_drive (corrected if clobbered by header)
//		pc_drive
//		pc_drive_type
//		pc_drive_heads
//		pc_drive_last_cylinder
//		pc_drive_last_sector
//	@clob	ax, cx, dx, di, si, es, flags
//	@since	IBM Personal Computer XT
dsknfo:	push	%bx
1:	push	%dx
	mov	$0x16,%ah		// make sure there is disk in drive,
	int	$0x13			// by querying change-line status
	jnc	2f
	xor	%ax,%ax			// if error or change line active,
	int	$0x13			// do a reset...
	jc	9f
	mov	$0x0201,%ax		// ...then do a read, to confirm that
	mov	$0x0001,%cx		// there is disk in drive
	mov	$0,%dh			// (verify (%ah = 4) does not work
	mov	$IMAGE_BASE_REAL>>4,%bx	// under QEMU, which always returns
	mov	%bx,%es			// success)
	xor	%bx,%bx
	int	$0x13
	jc	9f
2:	mov	$0x08,%ah		// get disk params
	int	$0x13
	jc	9f
	mov	%cl,%bh
	and	$0b00111111,%bh
	and	$0b11000000,%cl
	rol	%cl
	rol	%cl
	xchg	%cl,%ch
	push	%ds			// disk base table in es:di
	movpp	%es,%ds
	xor	%si,%si
	mov	%si,%es
	mov	$mm+"struct mman::pc_drive_base_table",%si
	xchg	%si,%di
	movsw	//→ headunloadtime, headloadtime
	movsw	//→ shutofftime, bytespersector
	movsw	//→ sectorspertrack, sectorgap
	movsw	//→ datalength, formatgap
	movsw	//→ formatfill, settletime
	movsb	//→ startuptime
	pop	%ds
	xchg	%bx,%ax
	stosw	//→ pc_drive_type, pc_drive_last_sector
	scasb
	xchg	%cx,%ax
	stosw	//→ pc_drive_last_cylinder
	xchg	%dx,%ax
	stosw	//→ pc_drives_attached, pc_drive_last_head
	pop	%ax
	stosb	//→ pc_drive
	xchg	%ax,%dx
	pop	%bx
	ret
9:	pop	%dx
8:	xor	$0x80,%dl		// try cycling drive a/c
	xor	%ax,%ax			// reset disk
	int	$0x13
	jc	8b
	jmp	1b
	.endfn	dsknfo

//	Reads disk sector via BIOS.
//
//	@param	al sector number
//	@param	es destination memory address >> 4
//	@param	cx cylinder number
//	@param	dh head number
//	@param	dl drive number
//	@return	number of sectors actually read
pcread:	push	%ax
	push	%cx
	xchg	%cl,%ch
	ror	%cl
	ror	%cl
	or	%al,%cl
	xor	%bx,%bx			// es:bx is destination addr
	mov	$1,%al			// read only one disk sector
	mov	$2,%ah			// read disk sectors ordinal
	int	$0x13
	pop	%cx
	pop	%ax
	jc	9f
	inc	%al			// ++sector
	cmp	mm+"struct mman::pc_drive_last_sector",%al
	jbe	2f
	mov	$1,%al
	inc	%dh			// ++head
	cmp	mm+"struct mman::pc_drive_last_head",%dh
	jbe	2f
	xor	%dh,%dh
	inc	%cx			// ++cylinder
2:	ret
9:	push	%ax
	xor	%ax,%ax			// try disk reset on error
	int	$0x13
	pop	%ax
	jmp	pcread
	.endfn	pcread

//	Video put string.
//
//	@param	di is the string
//	@clob	bp,bx
//	@mode	real
rvputs:	mov	%di,%si
0:	lodsb
	test	%al,%al
	je	1f
	mov	$7,%bx				// normal mda/cga style page zero
	mov	$0x0e,%ah			// teletype output al cp437
	int	$0x10				// vidya service
	jmp	0b
1:	ret
	.endfn	rvputs

//	Abnormally halts startup.
//
//	@param	di message
//	@mode	real
//	@noreturn
rldie:	push	%di
	mov	$REAL(str.error),%di
	call	rvputs
	pop	%di
	call	rvputs
	mov	$REAL(str.crlf),%di
	call	rvputs
0:	rep nop
	jmp	0b
	.endfn	rldie

//	Initializes present PC serial lines.
sinit4:	mov	$4,%cx
	mov	$0x400,%si			// BDA.COM1
0:	lodsw
	test	%ax,%ax
	jz	1f
	push	%cx
	push	%si
	xchg	%ax,%di
	mov	$REAL(sconf),%si
	call	sinit
	pop	%si
	pop	%cx
1:	loop	0b
	ret
	.endfn	sinit4,global,hidden

//	Initializes Serial Line Communications 8250 UART 16550A
//
//	@param	word di tty port
//	@param	char (*{es:,e,r}si)[4] register initial values
//	@mode	long,legacy,real
//	@see	www.lammertbies.nl/comm/info/serial-uart.html
sinit:	mov	%di,%dx
	test	%dx,%dx
	jz	2f
	push	%dx
	push	%si
	xor	%cx,%cx
	mov	$UART_LCR,%cl
	add	%cx,%dx
	lodsb	%ds:(%si),%al
	pop	%si
	or	$UART_DLAB,%al
	out	%al,%dx
	pop	%dx
1:	lodsb	%ds:(%si),%al
	out	%al,%dx
	inc	%dx
	dec	%cx
	jns	1b
2:	ret
	.endfn	sinit,global,hidden

/*───────────────────────────────────────────────────────────────────────────│─╗
│ αcτµαlly pδrταblε εxεcµταblε § partition table                           ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

//	Partition Table.
ape.mbrpad:
	.org	0x1b4
	.endobj	ape.mbrpad
ape_disk:
	.stub	ape.diskid,quad
	.org	0x1be,0x00
	.macro	.partn	x:req sta h0 s0 c0 fs h9 s9 c9 lba0 nsec
	.stub	ape.part\x\().status,byte,\sta	// 0=non-boot / 0x80=active
	.stub	ape.part\x\().first.head,byte,\h0
	.stub	ape.part\x\().first.sector,byte,\s0 # in low 6 bits
	.stub	ape.part\x\().first.cylinder,byte,\c0
	.stub	ape.part\x\().filesystem,byte,\fs
	.stub	ape.part\x\().last.head,byte,\h9
	.stub	ape.part\x\().last.sector,byte,\s9
	.stub	ape.part\x\().last.cylinder,byte,\c9
	.stub	ape.part\x\().lba,long,\lba0	// c₀*Cₙ + h₀*Hₙ + s₀*Sₙ
	.stub	ape.part\x\().sector.count,long,\nsec # sectors are 512 bytes
	.endm
	.partn	1,0x80,0,1,0,0x7f,0xff,0xff,0xff,0,0xffffffff
	.partn	2
	.partn	3
	.partn	4
	.org	0x1fe
	.short	BOOTSIG
	.endobj	ape_disk

#endif /* SupportsMetal() */

/*                                           ▄▄▄
                       ▄▄▄                    ▀▓▓▒▄
                     ▄▓▒▒░                      ▀▓▒▒▒▄
                   ▄▓▓▓▒▀              ▄▄▄▄      ▒▓▒▒░▒▄
                  ▄▓▓▓▒▓        ▄▄▓██▓▓▓▓▒▒▒▒▓▓▄▄▓▓▒▒▒░░▒
                  ▓▓▓▓▒▒▒▄▄  ░▒█▓▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▒▓▒░░▒░
                  ██▓▓▓▒▒░░▒▒▒▒▓▓▓▓▓▓▒▓▒░▒▒░▀▒▒▒▒░▀░▒▒▒░▒
                  ▓▓▓▓▓▓▓▒▒▒▒▒▒▓▓▒▓▓▒▒▒░▒▒░░  ░▒▒░  ░▒▒▒▒
                   ▀▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▒▒░░▒░░    ░▒▒  ░ ▀▒▒
                     ▀▓▓█▓▓▓▓▓▓▓▓▓▓▒▒░░▒▒░░   ░░░▓░ ▓░░░▒
                       ▀▀█▓███▓▓▓▓▓▒▒░░░▒░░  ░█▓░█▓░█▓▓▄▒░
                          ░▓██▓▓▓▓▓▒▒░░░▒░░  ░████▓▒▓█▓▀░▀▄
                          ░▓██▓▓▓▓▓▒▒▒░░░▒░░  ▒██▓▒▒▒▒▒▒░░░▒
                           ████▓▓▓▓▓▒▒▒▒▒▒▒▒▒░░▒▓▓▒░░░░▒░░░▒░ ░░░░░
                           ░▓███▓▓▓▓▓▒▒░░░░░░░▒▒▒▒▒▒▒▒▒▒▒░░░ ░░░░░   ░
                             ▓███▓▓▓▓▓▒▓▒▒▒▒░░░░░░░░░▒▓▒▒░▀ ░░░  ░░░░░
                              ▀▒██▓▓▓▓▒▒▒▓▓▓▓▒▒▒▒▒▒▒▓▀▀░    ░░░░░░░░░     ░
                                 ▓▓▓▓▓▓▓▒▓▒▒▒▒▓▓▓▒▀░ ░░░░░▄░░░  ░░░  ░░░░░░
                                 ▓▓▓▒▒▒▒▒▒▒▒▒▒▒▓     █▓▒░░▒░░░░ ░░░░░░░░
                                ▄▓▓▓▒▒▒▒▒░░░░░░░▒▄▄▄░▒▓▓▒▒░▀░
                               ░▓█▓▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓▓▓▓▓▓▒░░░▒  besiyata
                               ▓▓█▓▓▒▓▓▓▒▒▒░░░░░░▒▓▓▓▓▒▒▒▒▒░   dishmaya
                               ▓▓█▓▓▓▓▓▓▒▒▒░░░░░░░▒▓▓▒▀▀▀
                               ▓▓██▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▀
                                █▓▓█▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▀
                               ▒▓▓▓▓▀░░▒▓▓▓▓▓▓▓▓▒▒░░▒
                              ▄▓▓▀░░░▄▓▓▓▓▒▒▒▒▒░░░░▄░
                             ▄███▄▄▓▓▓▓▓▓▓▒▒▒▒▒░░▒▒░
                           ▄▓▓▓█▓█▓▓███▓▓▓▓▓▓▓▓▓▓▓░
                       ▄░▓▓▓▓▓▓▀▒▓▓▓▒▒▓▒░░░▒▓▒░░░▓
               ▄▄▄░▒▓▓▓▓▓▓░▀▀   ▓▓▒░▓▒▒▒▒▒▒▒▒▒▒▄░░▀▀░░ ▄▄▄▄
     ▄▄▄▒▒▓▓█▓▓▓▓▓▀▀▀▀▀        ▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▒▒▒▀░░▀░░▒▒▒░░░   ░░░░░
  ▄▓▓▓▒▀▀                      ▓▒▓▓▓▓▓▒▒▒▒▒▒▒▒▓░░░       ▒▒▒░░░░░░░░▒
  █▓▓▒      ▄▄▄                  ▀▓▒▓▒▒▒▓▓▓▓▓▓▒▒▒░░░░░░░░░▒▒░░░░░░░
   ▀▓▓▓▓▒▄▄▒▒▒▒▒▒▄▄                    ▀▀▀▀░░▒▒▒▒░░░░░░
       ▀▀▀▓▓▓▓▒▒▒▒▒▓▓▄▄
╔────────────────────────────────────────────────────────────────────────────│─╗
│ αcτµαlly pδrταblε εxεcµταblε § bell system five                          ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│─╝
	the bourne executable & linkable format */

#ifdef APE_IS_SHELL_SCRIPT
apesh:	.ascii	"\n@\n#'\"\n"			// sixth edition shebang
	.ascii	"m=$(uname -m 2>/dev/null) || m=x86_64\n"

	.ascii	"if [ \"$m\" = x86_64 ] || [ \"$m\" = amd64 ]; then\n"
//	Until all operating systems can be updated to support APE,
//	we have a beautiful, yet imperfect workaround, which is to
//	modify the binary to follow the local system's convention.
//	There isn't a one-size-fits-all approach for this, thus we
//	present two choices.
	.ascii	  "o=\"$(command -v \"$0\")\"\n"
//	Try to use system-wide APE loader.
	.ascii	  "[ x\"$1\" != x--assimilate ] && "
	.ascii	    "type ape >/dev/null 2>&1 && "
	.ascii	      "exec ape \"$o\" \"$@\"\n"
#ifdef APE_LOADER
//	There is no system-wide APE loader, but there is one
//	embedded inside the APE. So if the system is not MacOs,
//	extract the loader into a temp folder, and use it to
//	load the APE without modifying it.
	.ascii	  "[ x\"$1\" != x--assimilate ] && {\n"
	.ascii	    "t=\"${TMPDIR:-${HOME:-.}}/.ape-"
	.ascii	     APE_VERSION_STR
	.ascii	     "\"\n"
	.ascii	    "[ -x \"$t\" ] || {\n"
	.ascii	      "mkdir -p \"${t%/*}\" &&\n"
	.ascii	      "dd if=\"$o\" of=\"$t.$$\" skip="
	.shstub	        ape_loader_dd_skip,2
	.ascii	        " count="
	.shstub	        ape_loader_dd_count,2
	.ascii	        " bs=64 2>/dev/null\n"
#if SupportsXnu()
	.ascii	      "[ -d /Applications ] && "
	.ascii	        "dd if=\"$t.$$\""
	.ascii	          " of=\"$t.$$\""
	.ascii	          " skip=5"
	.ascii	          " count=8"
	.ascii	          " bs=64"
	.ascii	          " conv=notrunc"
	.ascii	          " 2>/dev/null\n"
#endif /* SupportsXnu() */
	.ascii	      "chmod 755 \"$t.$$\"\n"
	.ascii	      "mv -f \"$t.$$\" \"$t\"\n"
	.ascii	    "}\n"
	.ascii	    "exec \"$t\" \"$o\" \"$@\"\n"
	.ascii	  "}\n"
#endif /* APE_LOADER */
#ifndef APE_NO_MODIFY_SELF
//	The default behavior is: to overwrite the header in place.
//	We prefer this because it's a tiny constant one time cost.
//	We simply printf a 64-byte header and call execve() again.
#else
//	The alternative behavior is to copy to $TMPDIR or $HOME or
//	the current directory. We like TMPDIR because it's part of
//	the IEEE POSIX standard whereas alternatives (XDG) aren't.
	.ascii	  "t=\"${TMPDIR:-${HOME:-.}}/$0\"\n"
	.ascii	  "[ x\"$1\" != x--assimilate ] || [ ! -e \"$t\" ] && {\n"
	.ascii	    "[ x\"$1\" != x--assimilate ] && {\n"
	.ascii	      "mkdir -p \"${t%/*}\" 2>/dev/null\n"
	.ascii	      "cp -f \"$o\" \"$t.$$\" &&\n"
	.ascii	      "mv -f \"$t.$$\" \"$t\" || exit 120\n"
	.ascii	      "o=\"$t\"\n"
	.ascii	    "}\n"
#endif /* APE_NO_MODIFY_SELF */
	.ascii	  "exec 7<> \"$o\" || exit 121\n"
	.ascii	  "printf '"
	.ascii	    "\\177ELF"			// 0x0: ⌂ELF
	.ascii	    "\\2"			//   4: long mode
	.ascii	    "\\1"			//   5: little endian
	.ascii	    "\\1"			//   6: elf v1.o
	.ascii	    "\\011"			//   7: FreeBSD
	.ascii	    "\\0"			//   8: os/abi ver.
	.ascii	    "\\0\\0\\0"			//   9: padding 3/7
	.ascii	    "\\0\\0\\0\\0"		//      padding 4/7
	.ascii	    "\\2\\0"			//  10: εxεcµταblε
	.ascii	    "\\076\\0"			//  12: NexGen32e
	.ascii	    "\\1\\0\\0\\0"		//  14: elf v1.o
	.shstub	    ape_elf_entry,8		//  18: e_entry
	.shstub	    ape_elf_phoff,8		//  20: e_phoff
	.shstub	    ape_elf_shoff,8		//  28: e_shoff
	.ascii	    "\\0\\0\\0\\0"		//  30: e_flags
	.ascii	    "\\100\\0"			//  34: e_ehsize
	.ascii	    "\\070\\0"			//  36: e_phentsize
	.shstub	    ape_elf_phnum,2		//  38: e_phnum
	.ascii	    "\\0\\0"			//  3a: e_shentsize
	.shstub	    ape_elf_shnum,2		//  3c: e_shnum
	.shstub	    ape_elf_shstrndx,2		//  3e: e_shstrndx
	.ascii	    "' >&7\n"
	.ascii	  "exec 7<&-\n"
#if SupportsXnu()
	.ascii	  "[ -d /Applications ] && "
	.ascii	    "dd if=\"$o\""
	.ascii	      " of=\"$o\""
	.ascii	      " bs=8"
	.ascii	      " skip="
	.shstub	      ape_macho_dd_skip,2
	.ascii	      " count="
	.shstub	      ape_macho_dd_count,2
	.ascii	      " conv=notrunc 2>/dev/null\n"
#endif /* XNU */
	.ascii	  "[ x\"$1\" = x--assimilate ] && exit 0\n"
#ifndef APE_NO_MODIFY_SELF
	.ascii	  "exec \"$0\" \"$@\"\n"	// try to preserve argv[0]
#else
	.ascii	  "}\n"
	.ascii	  "o=\"$t\"\n"
	.ascii	  "exec \"$o\" \"$@\"\n"
#endif /* APE_NO_MODIFY_SELF */
	.ascii	  "exit $?\n"
	.ascii	"fi\n"				// x86_64
	.ascii	"echo error: this ape binary only supports x86_64 >&2\n"
	.ascii	"exit 1\n"
	.endobj	apesh

#ifdef APE_LOADER
        .section .ape.loader,"a",@progbits
	.balign	64
ape_loader:
        .incbin APE_LOADER
	.endobj	ape_loader,globl
	.balign	64
ape_loader_end:
	nop
	.endobj	ape_loader_end,globl
        .previous
#endif /* APE_LOADER */
#endif /* APE_IS_SHELL_SCRIPT */

#if SupportsSystemv() || SupportsMetal()
	.section .elf.phdrs,"a",@progbits

	.globl	ape_phdrs
ape_phdrs:
	.long	PT_LOAD
	.long	PF_R|PF_X
	.stub	ape_cod_offset,quad
	.stub	ape_cod_vaddr,quad
	.stub	ape_cod_paddr,quad
	.stub	ape_cod_filesz,quad
	.stub	ape_cod_memsz,quad
	.stub	ape_cod_align,quad

	.long	PT_LOAD
	.long	PF_R
	.stub	ape_rom_offset,quad
	.stub	ape_rom_vaddr,quad
	.stub	ape_rom_paddr,quad
	.stub	ape_rom_filesz,quad
	.stub	ape_rom_memsz,quad
	.stub	ape_rom_align,quad

	.long	PT_LOAD
	.long	PF_R|PF_W
	.stub	ape_ram_offset,quad
	.stub	ape_ram_vaddr,quad
	.stub	ape_ram_paddr,quad
	.stub	ape_ram_filesz,quad
	.stub	ape_ram_memsz,quad
	.stub	ape_ram_align,quad

//	These values are left empty because some UNIX OSes give p_filesz
//	priority over `ulimit -s` a.k.a. RLIMIT_STACK which is preferred
//	because we use an 8mb stack by default so that decadent software
//	doesn't unexpectedly crash, but putting that here meant NetBSD's
//	rusage accounting (which is the best) always reported 6mb of RSS
	.long	PT_GNU_STACK
	.stub	ape_stack_pf,long
	.quad	0
	.quad	0
	.quad	0
	.quad	0
	.quad	0
	.stub	ape_stack_align,quad

#if SupportsOpenbsd() || SupportsNetbsd()
	.long	PT_NOTE
	.long	PF_R
	.stub	ape_note_offset,quad
	.quad	0
	.quad	0
	.stub	ape_note_filesz,quad
	.stub	ape_note_memsz,quad
	.quad	4
#endif
	.previous
#endif /* SupportsSystemv() || SupportsMetal() */

	.section .note.ape.ident,"a",@progbits
	.balign	4
ape.ident:
	.long	2f-1f
	.long	4f-3f
	.long	1
1:	.asciz	"APE"
2:	.balign	4
3:	.long	APE_VERSION_NOTE
4:	.size	ape.ident,.-ape.ident
	.type	ape.ident,@object
	.previous

#if SupportsOpenbsd()
	.section .note.openbsd.ident,"a",@progbits
	.balign	4
openbsd.ident:
	.long	2f-1f
	.long	4f-3f
	.long	1
1:	.asciz	"OpenBSD"
2:	.balign	4
3:	.long	0
4:	.size	openbsd.ident,.-openbsd.ident
	.type	openbsd.ident,@object
	.previous
#endif /* SupportsOpenbsd() */

#if SupportsNetbsd()
	.section .note.netbsd.ident,"a",@progbits
	.balign	4
netbsd.ident:
	.long	2f-1f
	.long	4f-3f
	.long	1
1:	.asciz	"NetBSD"
2:	.balign	4
3:	.long	901000000
4:	.size	netbsd.ident,.-netbsd.ident
	.type	netbsd.ident,@object
	.previous
#endif /* SupportsNetbsd() */

/*	                                      ▄▄███▄
	                                  ▄▄████████▄
	                               ▄█████████████▄
	                           ▄▄███▓▓▓▓▓▓▓▓▓▓▓███▄
	                       ▄▄█████▓▓▓█████████▓▓▓██▄
	                   ▄▄████████▓▓▓███████▓▓▓▓▓████▄
	                ▄█████░░░████▓▓█████▓▓▓▓█████████▄
	            ▄▄█████████░░░███▓▓█▓▓▓▓▒███████▓▓▒███▄
	         ▄██████████████░░░██▓▓▓▓███████████▓▓█████▄
	       ██████████████████░░░██▓▓▓█████████▓▓▓███████▄
	        ███░░░░░░█████████▓░░███▓▓▓▓▓▓▓▓▓▓▓█████▒▒▒██▄
	       █░███░░░██░░░░░░░░░██░░██████████████▒▒▒▒██████▄
	      ███████░░░█████████░░░░░░█████████▒▒▒▒▒██████████▄
	      █████ ██░░░███████████████████▒▒▒▒▒██░▒▒██████████▄
	      ██████ ██░░░██████████████░███▒████████▒▒██████████▄
	     ████████ ███░░█████████████░░████████████▒▒███████████
	     █████████ ███░░███████████░░██████████████▒▒███████████
	    ▄██████████ ██████████████ ░░███████████████▒▒███████████
	    ████████████ ███░░░░░█████░░█████████████████▒▒██████  █
	    █████████████ ██████░░░░░░░▒█████████████████████  ████▀
	     █████████████ ██████████░░░░░░░░░███████████  ████████
	      █████████████ ████████░░███████░░░██████ ▓██████████
	       █████████████ ██████░░░████████████  █████████████
╔────────────────────────────────────────────────────────────────────────────│─╗
│ αcτµαlly pδrταblε εxεcµταblε § nexstep carnegie melon mach object format ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│─╝
	@note	hey xnu before we get upx'd email feedback jtunney@gmail.com
	@see	OS X ABI Mach-O File Format Reference, Apple Inc. 2009-02-04
	@see	System V Application Binary Interface NexGen32e Architecture
		Processor Supplement, Version 1.0, December 5th, 2018 */

#if SupportsXnu()
.section .macho,"a",@progbits
.balign	__SIZEOF_POINTER__

ape_macho:
	.long	0xFEEDFACE+1
	.long	MAC_CPU_NEXGEN32E
	.long	MAC_CPU_NEXGEN32E_ALL
	.long	MAC_EXECUTE
	.long	6				// number of load commands
	.long	70f-10f				// size of all load commands
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
	.stub	ape_cod_vaddr,quad
	.stub	ape_cod_memsz,quad
	.stub	ape_cod_offset,quad
	.stub	ape_cod_filesz,quad
	.long	PROT_EXEC|PROT_READ|PROT_WRITE	// maxprot
	.long	PROT_EXEC|PROT_READ		// initprot
	.long	0				// segment section count
	.long	0				// flags
30:	.long	MAC_LC_SEGMENT_64
	.long	40f-30b
	.ascin	"__RODATA",16
	.stub	ape_rom_vaddr,quad
	.stub	ape_rom_memsz,quad
	.stub	ape_rom_offset,quad
	.stub	ape_rom_filesz,quad
	.long	PROT_EXEC|PROT_READ|PROT_WRITE	// maxprot
	.long	PROT_READ			// initprot
	.long	0				// segment section count
	.long	0				// flags
40:	.long	MAC_LC_SEGMENT_64
	.long	50f-40b
	.ascin	"__DATA",16
	.stub	ape_ram_vaddr,quad
	.stub	ape_ram_memsz,quad
	.stub	ape_ram_offset,quad
	.stub	ape_ram_filesz,quad
	.long	PROT_EXEC|PROT_READ|PROT_WRITE	// maxprot
	.long	PROT_READ|PROT_WRITE		// initprot
	.long	0				// segment section count
	.long	0				// flags
50:	.long	MAC_LC_UUID
	.long	60f-50b
	.stub	ape_uuid1,quad
	.stub	ape_uuid2,quad
60:	.long	MAC_LC_UNIXTHREAD
	.long	70f-60b				// cmdsize
	.long	MAC_THREAD_NEXGEN32E		// flavaflav
	.long	(620f-610f)/4			// count
610:	.quad	0				// rax
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
620:
70:

.endobj	ape_macho,globl,hidden
.previous /* .macho */
#endif /* SupportsXnu() */

#if SupportsWindows() || SupportsMetal()
/*                             ░░░░
                        ▒▒▒░░░▒▒▒▒▒▒▒▓▓▓░
                       ▒▒▒▒░░░▒▒▒▒▒▒▓▓▓▓▓▓░
                      ▒▒▒▒░░░▒▒▒▒▒▒▒▓▓▓▓▓▓  ▒▓░
                      ▒▒▒░░░░▒▒▒▒▒▒▓▓▓▓▓▓   ▓▓▓▓▓▓▒        ▒▒▒▓▓█
                     ▒▒▒▒░░░▒▒▒▒▒▒▒▓▓▓▓▓▓  ▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓▓
                    ░▒▒▒░░░░▒▒▒▒▒▒▓▓▓▓▓▓   █▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓█
                    ▒▒▒▒░░░▒▒▒▒▒▒▒▓▓▓▓▓░  ▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓▓
                   ▒▒▒▒░░░▒▒▒▒▒▒▒▓▓▓▓▓▓  ▒▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓▒
                   ▒▒▒▒▓▓      ▓▒▒▓▓▓▓   ▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓█
                                    ▒▓  ▓▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓
                   ░░░░░░░░░░░▒▒▒▒      ▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓█
                 ▒▒░░░░░░░░░░▒▒▒▒▒▓▓▓     ▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓▓
                ░▒░░░░░░░░░░░▒▒▒▒▒▓▓   ▓░      ░▓███▓
                ▒▒░░░░░░░░░░▒▒▒▒▒▓▓░  ▒▓▓▓▒▒▒         ░▒▒▒▓   ████████████
               ▒▒░░░░░░░░░░░▒▒▒▒▒▓▓  ▒▓▓▓▓▒▒▒▒▒▒▒▒░░░▒▒▒▒▒░           ░███
               ▒░░░░░░░░░░░▒▒▒▒▒▓▓   ▓▓▓▓▒▒▒▒▒▒▒▒░░░░▒▒▒▒▓            ███
              ▒▒░░░░░░░░░░▒▒▒▒▒▒▓▓  ▒▓▓▓▒▒▒▒▒▒▒▒░░░░▒▒▒▒▒            ▓██
              ▒░░░░░░░░░░░▒▒▒▒▒▓▓   ▓▓▓▓▒▒▒▒▒▒▒▒░░░▒▒▒▒▒▓           ▓██
             ▒▒░░░▒▒▒░░░▒▒░▒▒▒▓▓▒  ▒▓▓▓▒▒▒▒▒▒▒▒░░░░▒▒▒▒▒           ███
                             ░▒▓  ░▓▓▓▓▒▒▒▒▒▒▒▒░░░░▒▒▒▒▓          ▓██
╔─────────────────────────────────────────────────────────────────▀▀▀────────│─╗
│ αcτµαlly pδrταblε εxεcµταblε § the new technology                        ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│─╝
	The Portable Executable Format

	@see https://docs.microsoft.com/en-us/windows/desktop/debug/pe-format
	@see "The Portable Executable File Format from Top to Bottom",
	     Randy Kath, Microsoft Developer Network Technology Group. */

//	    ┌14:Uniprocessor Machine            ┌─────────────────────────┐
//	    │┌13:DLL                            │ PE File Characteristics │
//	    ││┌12:System                        ├─────────────────────────┤
//	    │││┌11:If Net Run From Swap         │ r │ reserved            │
//	    ││││┌10:If Removable Run From Swap  │ d │ deprecated          │
//	    │││││┌9:Debug Stripped              │ D │ deprecated with     │
//	    ││││││┌8:32bit Machine              │   │ extreme prejudice   │
//	    │││││││  ┌5:Large Address Aware     └───┴─────────────────────┘
//	    │││││││  │   ┌1:Executable
//	    │││││││  │   │┌0:Relocs Stripped
//	   d│││││││dr│Ddd││
PEEXE = 0b00000001000100011

//	   ┌15:TERMINAL_SERVER_AWARE            ┌─────────────────────────┐
//	   │┌14:GUARD_CF                        │ PE DLL Characteristics  │
//	   ││┌13:WDM_DRIVER                     ├─────────────────────────┤
//	   │││┌12:APPCONTAINER                  │ r │ reserved            │
//	   ││││┌11:NO_BIND                      └───┴─────────────────────┘
//	   │││││┌10:NO_SEH
//	   ││││││┌9:NO_ISOLATION
//	   │││││││┌8:NX_COMPAT
//	   ││││││││┌7:FORCE_INTEGRITY
//	   │││││││││┌6:DYNAMIC_BASE
//	   ││││││││││┌5:HIGH_ENTROPY_VA
//	   │││││││││││rrrrr
DLLSTD = 0b0000000100100000
DLLPIE = 0b0000000001100000
DLLEXE = DLLSTD

//	   ┌31:Writeable                        ┌─────────────────────────┐
//	   │┌30:Readable                        │ PE Section Flags        │
//	   ││┌29:Executable                     ├─────────────────────────┤
//	   │││┌28:Shareable                     │ o │ for object files    │
//	   ││││┌27:Unpageable                   │ r │ reserved            │
//	   │││││┌26:Uncacheable                 └───┴─────────────────────┘
//	   ││││││┌25:Discardable
//	   │││││││┌24:Contains Extended Relocations
//	   ││││││││        ┌15:Contains Global Pointer (GP) Relative Data
//	   ││││││││        │       ┌7:Contains Uninitialized Data
//	   ││││││││        │       │┌6:Contains Initialized Data
//	   ││││││││ o      │       ││┌5:Contains Code
//	   ││││││││┌┴─┐rrrr│  ooror│││rorrr
PETEXT = 0b01100000000000000000000000100000
PERDAT = 0b01000000000000000000000001000000
PEDATA = 0b11000000000000000000000011000000
PEIMPS = 0b11000000000000000000000001000000

	.section .pe.header,"a",@progbits
	.balign	__SIZEOF_POINTER__
ape_pe:	.ascin	"PE",4
	.short	kNtImageFileMachineNexgen32e
	.short	ape_pe_shnum		// NumberOfSections
	.long	0x5c64126b		// TimeDateStamp
	.long	0			// PointerToSymbolTable
	.long	0			// NumberOfSymbols
	.short	ape_pe_optsz		// SizeOfOptionalHeader
	.short	PEEXE			// Characteristics
	.short	kNtPe64bit		// Optional Header Magic
	.byte	14			// MajorLinkerVersion
	.byte	15			// MinorLinkerVersion
	.long	0			// SizeOfCode
	.long	0			// SizeOfInitializedData
	.long	0			// SizeOfUninitializedData
	.long	RVA(ape_pe_entry)	// EntryPoint
	.long	0			// BaseOfCode
	.quad	ape_pe_base		// ImageBase
	.long	ape_pe_sectionalignment	// SectionAlignment
	.long	ape_pe_filealignment	// FileAlignment
	.short	v_ntversion		// MajorOperatingSystemVersion
	.short	0			// MinorOperatingSystemVersion
	.short	0			// MajorImageVersion
	.short	0			// MinorImageVersion
	.short	v_ntsubversion		// MajorSubsystemVersion
	.short	0			// MinorSubsystemVersion
	.long	0			// Win32VersionValue
	.long	RVA(_end)		// SizeOfImage
	.long	ape_pe_sizeofheaders	// SizeOfHeaders
	.long	0			// Checksum
	.short	v_ntsubsystem		// Subsystem: 0=Neutral,2=GUI,3=Console
	.short	v_ntdllchar		// DllCharacteristics
	.quad	0x10000			// StackReserve
	.quad	0x1000			// StackCommit
	.quad	0			// HeapReserve
	.quad	0			// HeapCommit
	.long	0			// LoaderFlags
	.long	2			// NumberOfDirectoryEntries
	.long	0,0			// ExportsDirectory
	.long	ape_idata		// ImportsDirectory
	.long	ape_idata_idtsize	// ImportsDirectorySize
	.endobj	ape_pe,globl
	.previous

	.section .pe.sections,"a",@progbits
	.ascin	".text",8		// Section Name
	.long	ape_text_memsz		// Virtual Size or Physical Address
	.long	ape_text_rva		// Relative Virtual Address
	.long	ape_text_filesz		// Physical Size
	.long	ape_text_offset		// Physical Offset
	.long	0			// Relocation Table Offset
	.long	0			// Line Number Table Offset
	.short	0			// Relocation Count
	.short	0			// Line Number Count
	.long	PETEXT			// Flags
	.previous

	.section .pe.sections,"a",@progbits
	.ascin	".rdata",8		// Section Name
	.long	ape_rom_memsz		// Virtual Size or Physical Address
	.long	ape_rom_rva		// Relative Virtual Address
	.long	ape_rom_filesz		// Physical Size
	.long	ape_rom_offset		// Physical Offset
	.long	0			// Relocation Table Offset
	.long	0			// Line Number Table Offset
	.short	0			// Relocation Count
	.short	0			// Line Number Count
	.long	PERDAT			// Flags
	.previous

	.section .pe.sections,"a",@progbits
	.ascin	".data",8		// Section Name
	.long	ape_ram_memsz		// Virtual Size or Physical Address
	.long	ape_ram_rva		// Relative Virtual Address
	.long	ape_ram_filesz		// Physical Size
	.long	ape_ram_offset		// Physical Offset
	.long	0			// Relocation Table Offset
	.long	0			// Line Number Table Offset
	.short	0			// Relocation Count
	.short	0			// Line Number Count
	.long	PEDATA			// Flags
	.previous

#endif /* SupportsWindows() || SupportsMetal() */

#if SupportsWindows()

	.section .idata.ro.idt.1,"a",@progbits
	.type	ape_idata_idtend,@object
	.type	ape_idata_idt,@object
	.globl	ape_idata_idt,ape_idata_idtend
	.hidden	ape_idata_idt,ape_idata_idtend
	.balign	4
ape_idata_idt:
	.previous/*
	...
	decentralized content
	...
	*/.section .idata.ro.idt.3,"a",@progbits
	.long	0,0,0,0,0
ape_idata_idtend:
	.byte	0
	.previous

	.section .piro.data.sort.iat.1,"aw",@progbits
	.type	ape_idata_iatend,@object
	.type	ape_idata_iat,@object
	.globl	ape_idata_iat,ape_idata_iatend
	.hidden	ape_idata_iat,ape_idata_iatend
	.balign	8
ape_idata_iat:
	.previous/*
	...
	decentralized content
	...
	*/.section .piro.data.sort.iat.3,"aw",@progbits
	.quad	0
ape_idata_iatend:
	.byte	0
	.previous

#endif /* SupportsWindows() */

#if SupportsMetal()

	.section .piro.data.sort.metal_gdt,"aw",@progbits
	.balign	8
_gdt:
//	          ┌G:granularity (1 → limit *= 0x1000)
//	          │┌D/B:default operation size (0 = 16|64bit, 1 = 32-bit)
//	          ││┌L:long mode
//	          │││┌AVL:this bit is thine (1<<52)
//	          ││││    ┌P:present
//	          ││││    │┌DPL:privilege
//	          ││││    ││ ┌─────────data/code(1)
//	          ││││    ││ │┌────data(0)──────code(1)
//	          ││││    ││ ││┌───conforming───expand-down
//	          ││││    ││ │││┌──writeable────readable
//	          ││││    ││ ││││┌─accessed─────accessed
//	          ││││    ││ │││││
//	          ││││ ┌──││─│││││───────────────────────────────┐
//	      ┌───││││─│──││─│││││───────────┐                   │
//	  ┌───┴──┐││││┌┴─┐│├┐│││││┌──────────┴───────────┐┌──────┴───────┐
//	  │      ││││││  ││││││││││          base address││ segment limit│
//	  │      ││││││  ││││││││││               32 bits││       20 bits│
//	  │      ││││││  ││││││││││                      ││              │
//	  6666555555555544444444443333333333222222222211111111110000000000
//	  3210987654321098765432109876543210987654321098765432109876543210
//	  │      ││││││  ││││││││││                      ││              │
.quad	0b0000000000000000000000000000000000000000000000000000000000000000 // 0
.quad	0b0000000000001111100110100000000000000000000000001111111111111111 // 8
.quad	0b0000000000001111100100100000000000000000000000001111111111111111 //16
.quad	0b0000000011001111100110100000000000000000000000001111111111111111 //24
.quad	0b0000000011001111100100100000000000000000000000001111111111111111 //32
.quad	0b0000000010101111100110110000000000000000000000001111111111111111 //40
.quad	0b0000000010101111100100110000000000000000000000001111111111111111 //48
.tssdescstub _tss							//56,64
_gdt_end:
	.endobj	_gdt,global,hidden
	.previous

/*─────────────────────────────────────────────────────────────────────────────╗
│ αcτµαlly pδrταblε εxεcµταblε § early-stage read-only data                    │
╚──────────────────────────────────────────────────────────────────────────────╝
	better code/data separation (.head is rwx[real] rx[long]) */

//	NUL-Terminated Strings.
str.error:
	.asciz	"error: "
	.endobj	str.error
str.crlf:
	.asciz	"\r\n"
	.endobj	str.crlf
str.e820:
	.asciz	"e820"
	.endobj	str.e820
str.oldcpu:
	.asciz	"oldcpu"
	.endobj	str.oldcpu

//	Serial Line Configuration (8250 UART 16550)
//	If it's hacked, it'll at least get hacked very slowly.
sconf:	.short	1843200/*hz*/ / 16/*wut*/ / 9600/*baud*/
//
//	          ┌interrupt trigger level {1,4,8,14}
//	          │ ┌enable 64 byte fifo (UART 16750+)
//	          │ │ ┌select dma mode
//	          │ │ │┌clear transmit fifo
//	          │ │ ││┌clear receive fifo
//	          ├┐│ │││┌enable fifos
	.byte	0b00000000
//
//	          ┌dlab: flips configuration mode state
//	          │┌enable break signal
//	          ││ ┌parity {none,odd,even,high,low}
//	          ││ │ ┌extra stop bit
//	          ││ │ │┌data word length (bits+5)
//	          ││┌┴┐│├┐
	.byte	0b01000011
	.endobj	sconf,global,hidden

//	Global Descriptor Table
	.balign	8
_gdtrlo:
	.short	2f-_gdtlo-1	// table byte length
	.long	REAL(_gdtlo)	// table address (base memory space)
	.endobj	_gdtrlo,global,hidden
_gdtr:
	.short	_gdt_end-_gdt-1	// table byte length
	.quad	_gdt		// table address (final virtual space)
	.endobj	_gdtr,global,hidden
	.balign	8
//	Partial GDT with descriptors for switching to unreal mode or long mode.
_gdtlo	= .-GDT_LEGACY_DATA
.quad	0b0000000011001111100100100000000000000000000000001111111111111111 #32
.quad	0b0000000010101111100110110000000000000000000000001111111111111111 #40
.quad	0b0000000010101111100100110000000000000000000000001111111111111111 #48
2:

/*─────────────────────────────────────────────────────────────────────────────╗
│ αcτµαlly pδrταblε εxεcµταblε § real mode                                     │
╚──────────────────────────────────────────────────────────────────────────────╝
	the default mode of operation on modern cpus */

realmodeloader:
	call	lhinit
	call	rlinit
	call	sinit4
	call	longmodeloader
	.endfn	realmodeloader

//	Prepares to later load parts of the program that are not loaded yet.
//
//	@param	al next sector number
//	@param	cx next cylinder number
//	@param	dh next head number
//	@param	dl drive number
//	@clob	ax, cx, es, di
lhinit:	cmp	$0x40,%dl
	je	9f
	pushpop	0,%es
	mov	$mm+"struct mman::pc_drive_next_sector",%di
	cld
	stosb	//→ pc_drive_next_sector
	xchg	%ax,%cx
	stosw	//→ pc_drive_next_cylinder
	mov	%dh,%al
	stosb	//→ pc_drive_next_head
9:	ret

	.section .sort.text.real.init.1,"ax",@progbits
	.type	rlinit,@function
rlinit:	.previous/*
	...
	decentralized function
	...
	*/.section .sort.text.real.init.3,"ax",@progbits
	ret
	.previous

/*                                                               █ █▒
                                                               █     █
                                        █▓▄                   █       █
                                       ▒▓  ░█░         ░▒▓▓███░        █▒
                                       ▒▓      ▒▓███▓▒░                 ░▓
                                        █                                ▓░
                                      ▄██░                                █
                                     ▓▓    ▓██░              ▓█░▓█        ▒▓
                                    ▒█   ░█    █            ░▓    █        █
                                    █░    █   ░▓             ▀▒  █░        █
                                    █▒     ▀▒░                             ▓▓
                        ▄▄▄▓████▓▓▒░ █                 ▄▄▄▄▓▓▓█▓▓▓▓▒░     ▒█
                    ▄▓▓▀              ▒       ░█     ▄▓▀                ░███▒
                   ▓▀                  ░░      ▀▓▄▄▄▒▒                     █
                   █                     ░░█▒       ▒█        ▒▓           █
                   ▀█       ▄▄▄▄▄▄▄▄▄▄               ▀▀░▓██▓               █
                     ▀■▄▄▄■▀          ▀▀█▄                                 █
                                         ▀▀█▄                              █
                                            ▀█▄                            █
                                              █▌                           █
                                              █▌                           █
                                              █▌                           █
                                              █▌                           █
                                              █▌                           █
                                              █▌                           █
                                              █▌                           █
                                              █▌                           █
                                              █▌                           █
                                              █▌                           █
                                              █▌                           █
                                              █▌                           █
                                              █▌                           █
                                              █▌                           █
                                              █▌                           █
                                              █▌                           █
                                              █▌                           █
                                              █▌                           █
                                              █▌                           █
                                              █▌                           █
                                              █▌                           █
                                              █▌                           █
╔──────────────────────────────────────────────────────────────────────────▀─│─╗
│ αcτµαlly pδrταblε εxεcµταblε § long mode loader                          ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│─╝
	long mode is long */

longmodeloader:
	call	lcheck
	call	a20
	call	e820
	call	cpyhi
	call	loadhi
	call	pinit
	call	golong
	.endfn	longmodeloader

//	Long Mode Hardware Check
lcheck:	pushf				// checks for i8086 / i8088 / i80186
	pop	%ax			// see intel manual volume 1 §20.1.2
	test	$0x80,%ah
	jnz	10f			// we now assume 32 bit is supported
	pushfl				// now check for i386, or early i486
	pop	%eax			// tests ability to change cpuid bit
	mov	%eax,%ecx
	mov	$1<<21,%ebx
	xor	%ebx,%eax
	push	%eax
	popfl
	pushfl
	pop	%eax
	cmp	%eax,%ecx
	je	10f			// we assume cpuid inst is available
	or	%ebx,%eax		// puts cpuid bit in the on position
	push	%eax
	popfl
	mov	$0x80000000,%edi	// get amd64 ext cpuid thingy length
	mov	%edi,%eax
	inc	%edi
	cpuid				// leaf 0x80000000, clob ax/bx/cx/dx
	cmp	%edi,%eax
	jl	10f
	mov	%edi,%eax
	cpuid				// leaf 0x80000001, clob ax/bx/cx/dx
	mov	$1<<29|1<<20,%edi	// 29 = LM (long), 20 = NX (no exec)
	and	%edi,%edx		// we need to both, or we won't work
	cmp	%edi,%edx
	jne	10f
	xor	%ax,%ax
1:	ret
10:	mov	$REAL(str.oldcpu),%di
20:	call	rldie
	.endfn	lcheck

//	Gets memory map from BIOS.
e820:	xor	%ebx,%ebx		// %ebx is an api state tracker
	mov	%bx,%es
	mov	$mm+"struct mman::e820",%di # es:di is destination buffer
1:	mov	$0xE820,%eax		// magic
	mov	$8+8+4+4,%ecx		// sizeof(struct SmapEntry)
	mov	$0x534d4150,%edx	// magic number
	movl	$1,8+8+4/*SmapEntry::acpi3*/(%di) # prefill ACPI attributes;
					// apparently some buggy BIOSes say
					// that they return this field, yet
					// do not fill it correctly
	int	$0x15			// ax,bx,cx,dx,di → ax,bx,cx
	jc	9f			// cf = unsupported or abuse
	cmp	%edx,%eax		// more magic means success
	jne	9f
	test	%cx,%cx			// discard empty results
	jz	5f
	mov	8/*LODWORD(SmapEntry::size)*/(%di),%eax
	or	8+4/*HIDWORD(SmapEntry::size)*/(%di),%eax
	jz	5f
	cmp	$8+8+4+1,%cx		// discard if ignore flag
	jb	4f
	testb	$1/*don't ignore*/,8+8+4/*SmapEntry::acpi3*/(%di)
	jz	5f
4:	add	$8+8+4+4,%di		// keep entry
5:	test	%ebx,%ebx		// last entry?
	jz	8f
	cmp	$mm+"struct mman::e820_end"-(8+8+4+4),%di
	jb	1b
8:	xor	%ax,%ax			// add a blank sentinel entry
	mov	$(8+8)/2,%cx
	cld
	rep stosw
	ret
9:	mov	$REAL(str.e820),%di
	call	rldie
	.endfn	e820

//	Asks keyboard to grant system 65,519 more bytes of memory.
//
//	Yup.
//
//	@assume	realmode && df=0
//	@clob	ax,di,si,es,flags
//	@mode	real
//	@see	wiki.osdev.org/A20_Line
a20:	cli
	push	%ds
	xor	%ax,%ax
	mov	%ax,%es
	dec	%ax
	mov	%ax,%ds
	mov	$0x0500,%di
	mov	$0x0510,%si
	mov	%es:(%di),%al
	push	%ax
	mov	%ds:(%si),%al
	push	%ax
	movb	$0x00,%es:(%di)
	movb	$0xff,%ds:(%si)
	cmpb	$0xff,%es:(%di)
	pop	%ax
	mov	%al,%ds:(%si)
	pop	%ax
	mov	%al,%es:(%di)
	pop	%ds
	jne	3f
	mov	$1,%ax
	call	1f
	mov	$0xad,%al
	out	%al,$0x64
	call	1f
	mov	$0xd0,%al
	out	%al,$0x64
	call	2f
	in	$0x60,%al
	push	%ax
	call	1f
	mov	$0xd1,%al
	out	%al,$0x64
	call	1f
	pop	%ax
	or	$2,%al
	out	%al,$0x60
	call	1f
	mov	$0xae,%al
	out	%al,$0x64
	call	1f
	jmp	a20
1:	in	$0x64,%al
	test	$2,%al
	jnz	1b
	ret
2:	in	$0x64,%al
	test	$1,%al
	jz	2b
	ret
3:	sti
5:	ret
	.endfn	a20

//	Copies program pages loaded into base memory, to extended memory.
//
//	@clob	esi, edi
cpyhi:	push	%es
	movpp	%ds,%es
	call	unreal
	mov	$IMAGE_BASE_REAL,%esi
	mov	$IMAGE_BASE_PHYSICAL,%edi
	mov	$v_ape_realdwords,%ecx
	cld
	rep movsl %ds:(%esi),%es:(%edi)
	sti
	pop	%es
	ret
	.endfn	cpyhi

//	Disables interrupts and switches to "unreal mode".
//
//	@return	ds, es have same base addresses as before but can access
//		4 GiB of memory
//	@clob	eax
unreal:	push	%ds
	push	%es
	cli
	lgdt	REAL(_gdtrlo)
	mov	%cr0,%eax
	or	$CR0_PE,%al
	mov	%eax,%cr0
	jmp	0f
0:	pushpop	GDT_LEGACY_DATA,%ds
	movpp	%ds,%es
	and	$~CR0_PE,%al
	mov	%eax,%cr0
	jmp	1f
1:	pop	%es
	pop	%ds
	ret

//	Reads any remaining program pages into memory which have not yet
//	been read by the boot sector.
//
//	@clob	eax, ecx, dx, esi, edi, bp
loadhi:	mov	$v_ape_highsectors,%bp
	test	%bp,%bp
	jz	9f
	mov	$mm+"struct mman::pc_drive",%si
	cld
	lodsb	//← pc_drive
	xchg	%ax,%dx
	lodsw
	lodsb	//← pc_drive_next_sector
	test	%al,%al
	jz	9f
	xchg	%ax,%cx
	lodsw	//← pc_drive_next_cylinder
	xchg	%ax,%cx
	mov	(%si),%dh #← pc_drive_next_head
	push	%es
#define	SEG 0x79000
	mov	$IMAGE_BASE_PHYSICAL+v_ape_realbytes-SEG,%edi
	push	$SEG>>4
	pop	%es
0:	call	pcread
	push	%ax
	call	unreal
	pop	%ax
	push	%cx
	xor	%esi,%esi
	xor	%ecx,%ecx
	mov	$512/4,%cl
	cld
	rep movsl %es:(%esi),%es:(%edi)
	sti
	pop	%cx
	dec	%bp
	jnz	0b
	pop	%es
9:	ret

//	Initializes long mode paging.
pinit:	push	%ds
	push	%es
	mov	$SEG>>4,%ax
	mov	%ax,%ds
	mov	%ax,%es
	xor	%di,%di
	xor	%ax,%ax
	mov	$(0x7f000-SEG)/2,%cx
	cld
	rep stosw
	movl	$0x7d000+PAGE_V+PAGE_RW,0x7e000-SEG	// PDPT←PML4T (+)
	movl	$0x7c000+PAGE_V+PAGE_RW,0x7e800-SEG	// PDPT←PML4T (-)
	movl	$0x7b000+PAGE_V+PAGE_RW,0x7d000-SEG	// PDT←PDPT   (+)
	movl	$0x7a000+PAGE_V+PAGE_RW,0x7c000-SEG	// PDT←PDPT   (-)
	movl	$0x79000+PAGE_V+PAGE_RW,0x7b000-SEG	// PD←PDT     (+)
	movl	$0x79000+PAGE_V+PAGE_RW,0x7a000-SEG	// PD←PDT     (-)
	mov	$512,%cx				// PD±2MB
	mov	$PAGE_V+PAGE_RSRV+PAGE_RW,%eax
	xor	%di,%di
0:	stosl
	add	$0x1000,%eax
	scasl						// di += 4
	loop	0b
	mov	$0x7e000,%eax				// PML4T←CR3
	mov	%eax,%cr3
	pop	%es
	pop	%ds
	ret
	.endfn	pinit

//	Switch from Real Mode → Long Mode
//
//	@see	Intel Manual V3A §4.1.2
golong:	cli
	lidt	mm+"struct mman::bad_idt"
	mov	%cr4,%eax
	or	$CR4_PAE|CR4_PGE|CR4_OSFXSR,%eax
	mov	%eax,%cr4
	movl	$EFER,%ecx
	rdmsr
	or	$EFER_LME|EFER_SCE|EFER_NXE,%eax
	wrmsr
	lgdt	REAL(_gdtrlo)
	mov	%cr0,%eax
	or	$CR0_PE|CR0_PG|CR0_MP,%eax
	and	$~CR0_EM,%eax
	mov	%eax,%cr0
	ljmp	$GDT_LONG_CODE,$REAL(long)
	.endfn	golong

//	Long mode is long.
	.code64
long:	movabs	$BANE+PHYSICAL(0f),%rax
	jmp	*%rax
0:	xor	%eax,%eax
	mov	$GDT_LONG_DATA,%al
	mov	%eax,%ds
	mov	%eax,%ss
	mov	%eax,%es
	mov	%eax,%fs
	mov	%eax,%gs
	mov	$0x80000,%esp
	xor	%r12d,%r12d
	xor	%r13d,%r13d
	xor	%r14d,%r14d
	xor	%r15d,%r15d
	xor	%ebx,%ebx
	xor	%ebp,%ebp
	mov	$mm,%rdi
	mov	%cr3,%rsi
	mov	$IMAGE_BASE_PHYSICAL,%edx
	lea	v_ape_allbytes(%rdx),%ecx
	call	__map_phdrs
	push	$0x037f
	fldcw	(%rsp)
	lgdt	_gdtr					// reload GDTR for
							// virtual memory space
	movabs	$kernel,%rax
	jmp	*%rax
	.endfn	long

/*─────────────────────────────────────────────────────────────────────────────╗
│ αcτµαlly pδrταblε εxεcµταblε § multiboot stub                                │
╚──────────────────────────────────────────────────────────────────────────────╝
	boot modernized for the nineties */

#define GRUB_MAGIC 0x1BADB002
#define GRUB_EAX 0x2BADB002
#define GRUB_AOUT (1 << 16)
#define GRUB_CHECKSUM(FLAGS) (-(GRUB_MAGIC + (FLAGS)) & 0xffffffff)

//	Grub Header.
	.balign	4
ape_grub:
	.long	GRUB_MAGIC			// Magic
	.long	GRUB_AOUT			// Flags
	.long	GRUB_CHECKSUM(GRUB_AOUT)	// Checksum
	.long	RVA(ape_grub)			// HeaderPhysicalAddress
	.long	IMAGE_BASE_PHYSICAL		// TextPhysicalAddress
	.long	PHYSICAL(_edata)		// LoadEndPhysicalAddress
	.long	PHYSICAL(_end)			// BssEndPhysicalAddress
	.long	RVA(ape_grub_entry)		// EntryPhysicalAddress
	.endobj	ape_grub,globl

//	Grub Entrypoint.
//	Takes CPU out of legacy mode and jumps to normal entrypoint.
//	@noreturn
	.balign	4
ape_grub_entry:
	.code32
//	cmp	$GRUB_EAX,%eax
//	jne	triplf
	push	$0
	popf
	mov	$0x40,%dl
	mov	%cr0,%eax
	and	$~CR0_PE,%eax
	mov	%eax,%cr0
	ljmpw	$0,$REAL(pc)
	.code64
	.endfn	ape_grub_entry

/*  ▄▄▒▀▀▀▀▒▒▄
█████▓▓▄░░░░  ▒▒▄
▐█▓▓█▓▄█████▓░   ▀▒▄
 ▓█▓▓▓▓▓▓▓▓▓█▓     ░▀▒░
   ▀▀▓█▓▓▓▓▓▓█▓       ░▀▒▄▄▒▄▄▄▒▒▒▀▀▀▀▀▀▀▀▀▀▀▀▀▀▒▒▄▒
       ▀▓▓█▓▓▓▓▓▄          ░                       ░▀▒▄▄                       ▄
           ▀█▓▓▓▓▓▄░                                   ░▀▒░                  ▄▓▌
            ▀█▓▓▓▓▓█▓░                                    ░▀▄░            ░▄▓██░
              ▀█▓▓▓▓▓█▄                                      ░▀▒▄▄▄▄▄▄▄▄▒▀▓█▓█▓░
                ▀█▓▓▓▓█▓░                                             ░▄▓█▓▓█▓▄▌
                  ▀█▓▓▓▓▓▄                        ▄▄▄▓▓██▓▓▒▒░      ▒▒▄▓█▓▓▓▓█▓
                   ▀█▓▓▓▓▓▓▄▄░               ░▄▄▓██▓▓▓▓▓██████████████▓▓▓▓▓▓█▀
                    ▐█▓▓▓▓▓▓▓██▓▓▄▄▄▄▄▄▄▄▄▓▓██▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓█████████▓▓▀
                     ▀▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓█░ ░░░░░░
                     ▄░▓█▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓█▌
                     █▓▄▀▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓██▓▓▀▀▀▀▀▀▀▓█▓▓▓▓▓▓▓▓▓▓▓▓
                    ▐█▓█▓░▓█▓▓▓▓▓▓▓█▀▀▀▀▒░  ▐▓▓▓▓▓▓▄▒▀▓█▓▓▓▓▓▓▓▓█▌
                    ▐█▓▓▓█▒▀█▓▓▓▓▓▓█▌        ▀█▓▓▓▓▓█▄░▀▓▓█▓▓▓▓▓▓█░
                    █▓▓▓▓▓█░▐█▓▓▓▓▓█▌         ▀▓█▓▓▓▓█▌   ░▀█▓▓▓▓▓█▄
                    █▓▓▓▓█▌ ░█▓▓▓▓▓█░           ▓█▓▓▓▓▓░    ▒█▓▓▓▓▓█░
                    █▓▓▓▓█░  █▓▓▓▓▓█░            █▓▓▓▓▓█░    ▐█▓▓▓▓█▌
                    ▐█▓▓▓▓▓▄ █▓▓▓▓▓█         ░▄▄▓█▓▓▓▓▓█░    ▐█▓▓▓▓█░
                     ▓█▓▓▓█▓▐█▓▓▓▓█▌        ▓██▓▓▓▓▓▓█▀      ▓█▓▓▓▓█
                      ▀▓██▒▄█▓▓▓▓▓█░       ░█▓▓▓▓▓██▀░     ░▓█▓▓▓▓█▌
                         ░░█▓▓▓▓▓█▓         ░▀▀▀▀▀▀        ▓█▓▓▓▓▓▓
╔───────────────────────────────────────────────────────────▀▀▀▀▀▀───────────│─╗
│ αcτµαlly pδrταblε εxεcµταblε § cosmopolitan libc runtime runtime         ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/
kernel:	movabs	$ape_stack_vaddr,%rsp
	add	$ape_stack_memsz,%rsp
	movl	$0,0x7b000			// unmap null 2mb
#if USE_SYMBOL_HACK
	.byte	0x0f,0x1f,0207			// nop rdi binbase
	.long	(IMAGE_BASE_VIRTUAL-IMAGE_BASE_REAL)/512
#endif
	movabs	$BANE+mm,%rdi
	mov	$0x79000,%esi
	mov	$0x7f000,%edx
	call	__reclaim_boot_pages
	push	$_HOSTMETAL			// sets __hostos in crt.S
	pop	%rcx
	pushq	.Lenv0(%rip)			// envp[0][0]
	mov	%rsp,%rbp
	pushq	.Largv0+8(%rip)			// argv[0][8]
	pushq	.Largv0(%rip)			// argv[0][0]
	mov	%rsp,%rax
	push	$0				// auxv[1][1]
	push	$0				// auxv[1][0]
	push	%rax				// auxv[0][1]
	push	$31				// auxv[0][0] AT_EXECFN
	push	$0				// envp[1]
	push	%rbp				// envp[0]
	push	$0				// argv[1]
	push	%rax				// argv[0]
	push	$1				// argc
	xor	%ebp,%ebp
	xor	%eax,%eax
	xor	%edx,%edx
	xor	%edi,%edi
	xor	%esi,%esi
	xor	%r8d,%r8d
	xor	%r9d,%r9d
	xor	%r10d,%r10d
	xor	%r11d,%r11d
	jmp	_start
	.endfn	kernel

	.rodata
.Lenv0:	.asciz	"METAL=1"
.Largv0:
	.asciz	APE_COM_NAME
	.org	.Largv0+16
	.previous

#endif /* SupportsMetal() */

//	Avoid linker script variables appearing as code in objdump.
	.macro	.ldsvar	name:req
	.type	\name,@object
	.weak	\name
	.endm
	.ldsvar	_end
	.ldsvar	_etext
	.ldsvar	v_ape_realsectors
	.ldsvar	v_ape_realbytes
	.ldsvar	v_ape_highsectors
	.ldsvar	ape_idata_ro
	.ldsvar	ape_piro
	.ldsvar	ape_piro_end
	.type	ape_macho_end,@object
	.type	ape_note,@object
	.type	ape_note_end,@object
	.type	ape_note_vaddr,@object
	.type	ape_phdrs,@object
	.type	ape_pe_sections,@object
	.type	ape_pe_sections_end,@object
	.type	ape_text_nops,@object
	.type	__test_end,@object

	.section .commentprologue,"a",@progbits
	.globl	__comment_start
	.type	__comment_start,@object
	.hidden	__comment_start
__comment_start:/*
	...
	decentralized content
	...
	*/.previous
	.section .commentepilogue,"a",@progbits
	.byte	0
	.previous

	.section .ape.pad.head,"a",@progbits
	.type	ape_pad_head,@object
	.hidden	ape_pad_head
ape_pad_head:
	.previous

	.section .ape.pad.text,"a",@progbits
	.type	ape_pad_text,@object
	.hidden	ape_pad_text
ape_pad_text:
	.previous

	.section .ape.pad.privileged,"a",@progbits
	.type	ape_pad_privileged,@object
	.hidden	ape_pad_privileged
ape_pad_privileged:
	.previous

	.section .ape.pad.data,"a",@progbits
	.type	ape_pad_data,@object
	.hidden	ape_pad_data
ape_pad_data:
	.previous

#if SupportsWindows()
	.section .idata.ro,"a",@progbits
	.type	ape_idata_ro,@object
	.hidden	ape_idata_ro
ape_idata_ro:
	.previous
#endif /* SupportsWindows() */

	.section .dataprologue,"aw",@progbits
	.type	__data_start,@object
	.globl	__data_start
	.hidden	__data_start
__data_start:
	.previous

	.section .dataepilogue,"aw",@progbits
	.type	__data_end,@object
	.globl	__data_end
	.hidden	__data_end
__data_end:
	.previous

	.section .bssprologue,"aw",@nobits
	.type	__bss_start,@object
	.globl	__bss_start
	.hidden	__bss_start
__bss_start:
	.previous

	.section .bssepilogue,"aw",@nobits
	.type	__bss_end,@object
	.globl	__bss_end
	.hidden	__bss_end
__bss_end:
	.previous

	.section .fstls,"awT",@nobits
	.align	TLS_ALIGNMENT
	.previous

.end
