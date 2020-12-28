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
#include "ape/config.h"
#include "ape/lib/pc.h"
#include "ape/macros.internal.h"
#include "ape/notice.inc"
#include "ape/relocations.h"
#include "libc/elf/def.h"
#include "libc/macho.internal.h"
#include "libc/nexgen32e/uart.internal.h"
#include "libc/nexgen32e/vidya.internal.h"
#include "libc/nt/pedef.internal.h"
#include "libc/dce.h"
#include "libc/sysv/consts/prot.h"

#define USE_SYMBOL_HACK 0

	.source	"NOTICE"
	.source	"ape/ape.S"
	.source	"ape/ape.lds"
	.section .text,"ax",@progbits
	.align	__SIZEOF_POINTER__
	.previous
	.section .rodata,"a",@progbits
	.align	__SIZEOF_POINTER__
__ro:	.endobj	__ro,globl,hidden		# ←for gdb readibility
	.previous
	.section .data,"aw",@progbits
	.align	__SIZEOF_POINTER__
	.previous
	.section .bss,"aw",@nobits
	.align	__SIZEOF_POINTER__
	.previous
	.section .rodata.str1.1,"aMS",@progbits
cstr:	.endobj	cstr,globl,hidden		# ←for gdb readibility
	.previous
	.section .sort.rodata.real.str1.1,"aMS",@progbits
rlstr:	.endobj	rlstr,globl,hidden		# ←for gdb readibility
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

/	MZ Literally Executable Header
/
/	This is the beginning of the program file and it can serve as an
/	entrypoint too. It shouldn't matter if the program is running on
/	Linux, Windows, etc. Please note if the underlying machine isn't
/	a machine, this header may need to morph itself to say the magic
/	words, e.g. ⌂ELF, which also works fine as a generic entrypoint.
/
/	@see	www.delorie.com/djgpp/doc/exe/
/	@noreturn
ape.mz:	.ascii	"MZ"			# Mark 'Zibo' Joseph Zbikowski
	jno	2f			# MZ: bytes on last page
	jo	2f			# MZ: 512-byte pages in file
	.ascii	"='"			# MZ: reloc table entry count
	.ascii	"\n\0"			# MZ: data segment file offset / 16
	.short	0x1000			# MZ: lowers upper bound load / 16
	.short	0xf800			# MZ: roll greed on bss
	.short	0			# MZ: lower bound on stack segment
	.short	0			# MZ: initialize stack pointer
	.short	0			# MZ: ∑bₙ checksum don't bother
	.short	0x0100			# MZ: initial ip value
	.short	0x0800			# MZ: increases cs load lower bound
	.short	0x0040			# MZ: reloc table offset
	.short	0			# MZ: overlay number
	.org	0x24			# MZ: bytes reserved for you
	.ascii	"JT"			# MZ: OEM identifier
	.short	0			# MZ: OEM information
	.org	0x40-4			# MZ: bytes reserved for you
	.long	RVA(ape.pe)		# PE: the new technology
	.endfn	ape.mz,globl,hidden

/	Disk Operating System Stub
/	@noreturn
	.org	0x40			# mz/elf header length
stub:	mov	$0x40,%dl		# *literally* dos
	jmp	1f			# good bios skips here
1:	jmp	pc			# thus avoiding heroics
	nop				# system five bootpoint
	.org	0x48,0x90		# note ⌂ELF means JG 47
	jmp	3f			# MZ also means pop r10
2:	sub	$8,%rsp			# a.k.a. dec %ax sub %sp
	xor	%edx,%edx		# MZ ate BIOS drive code
3:	.byte	0xbd,0,0		# a.k.a. mov imm,%bp
	jmp	pc			# real mode, is real
	jmp	_start			# surprise it's unix
	.endfn	stub

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

/	Initializes program and jumps to real mode loader.
/
/	@param	dl drive number (use 0x40 to skip bios disk load)
/	@mode	real
/	@noreturn
	.code16
pc:	cld
#if USE_SYMBOL_HACK
	.byte	0x0f,0x1f,0207			# nop rdi binbase
	.short	(0x7c00-IMAGE_BASE_VIRTUAL)/512
#endif
	mov	$REAL_STACK_FRAME>>4,%di	# we need a stack
	xor	%cx,%cx
	mov	%cx,%es
	rlstack	%di,%cx
	push	%cs				# memcpy() [relocate this page]
	pop	%ds
	call	1f
1:	pop	%si
	sub	$RVA(1b),%si
	mov	$IMAGE_BASE_REAL>>4,%ax
	push	%ax				# save real base
	push	%ax
	pop	%es
	xor	%di,%di
	mov	$512,%cx
	rep movsb
#if USE_SYMBOL_HACK
	.byte	0x0f,0x1f,0207			# nop rdi binbase
	.short	(IMAGE_BASE_REAL-0x7c00)/512
#endif
	ljmp	$0,$REAL(1f)			# longjmp()
1:	mov	%cx,%ds				# %ds and %cs are now zero
	mov	$XLM_SIZE,%cx			# memset to clear real bss
	mov	$XLM_BASE_REAL>>4,%ax
	mov	%ax,%es
	xor	%ax,%ax
	xor	%di,%di
	rep stosb
	cmp	$0x40,%dl			# statfs() [disk geometry]
	je	6f
	call	dsknfo
	pop	%es				# restore real base
	mov	$1,%al				# current sector
	xor	%cx,%cx				# current cylinder
	xor	%dh,%dh				# current head
	mov	$v_ape_realsectors,%di		# total sectors
3:	call	pcread
	dec	%di
	jnz	3b
6:	mov	$XLM(LOADSTATE),%di		# ax,cx,dx,es
	stosw
	xchg	%cx,%ax
	stosw
	xchg	%dx,%ax
	stosw
	mov	%es,%ax
	stosw
	ljmp	$0,$REAL(realmodeloader)
	.endfn	pc,globl,hidden

/	Determines disk geometry.
/
/	We use imperial measurements for storage systems so the software
/	can have an understanding of physical locality, which deeply
/	impacts the latency of operations.
/
/	 - 160KB:  1 head  × 40 cylinders ×  8 sectors × 512 =   163,840
/	 - 180KB:  1 head  × 40 cylinders ×  9 sectors × 512 =   184,320
/	 - 320KB:  2 heads × 40 cylinders ×  8 sectors × 512 =   327,680
/	 - 360KB:  2 heads × 40 cylinders ×  9 sectors × 512 =   368,640
/	 - 720KB:  2 heads × 80 cylinders ×  9 sectors × 512 =   737,280
/	 - 1.2MB:  2 heads × 80 cylinders × 15 sectors × 512 = 1,228,800
/	 - 1.44MB: 2 heads × 80 cylinders × 18 sectors × 512 = 1,474,560
/
/	Terminology
/
/	 - Cylinder / Tracks should mean the same thing
/	 - Heads / Sides / Spindles should mean the same thing
/
/	Disk Base Table
/
/	   0: specify byte 1, step-rate time, head unload time
/	   1: specify byte 2, head load time, DMA mode
/	   2: timer ticks to wait before disk motor shutoff
/	   3: bytes per sector code
/	        0: 128 bytes  2: 512 bytes
/	        1: 256 bytes  3: 1024 bytes
/	   4: sectors per track (last sector number)
/	   5: inter-block gap length/gap between sectors
/	   6: data length, if sector length not specified
/	   7: gap length between sectors for format
/	   8: fill byte for formatted sectors
/	   9: head settle time in milliseconds
/	  10: motor startup time in eighths of a second
/
/	@param	dl drive number
/	@return	dl = pc_drive (corrected if clobbered by header)
/		pc_drive
/		pc_drive_type
/		pc_drive_heads
/		pc_drive_last_cylinder
/		pc_drive_last_sector
/	@clob	ax, cx, dx, di, si, es, flags
/	@since	IBM Personal Computer XT
dsknfo:	push	%bx
1:	push	%dx
	mov	$0x08,%ah		# get disk params
	int	$0x13
	jc	9f
	mov	%cl,%bh
	and	$0b00111111,%bh
	and	$0b11000000,%cl
	rol	%cl
	rol	%cl
	xchg	%cl,%ch
	push	%ds			# disk base table in es:di
	movpp	%es,%ds
	xor	%si,%si
	mov	%si,%es
	mov	$XLM(DRIVE_BASE_TABLE),%si
	xchg	%si,%di
	movsw	#→ headunloadtime, headloadtime
	movsw	#→ shutofftime, bytespersector
	movsw	#→ sectorspertrack, sectorgap
	movsw	#→ datalength, formatgap
	movsw	#→ formatfill, settletime
	movsb	#→ startuptime
	pop	%ds
	xchg	%bx,%ax
	stosw	#→ pc_drive_type, pc_drive_last_sector
	xchg	%cx,%ax
	stosw	#→ pc_drive_last_cylinder
	xchg	%dx,%ax
	stosw	#→ pc_drives_attached, pc_drive_last_head
	pop	%ax
	stosb	#→ pc_drive
	xchg	%ax,%dx
	pop	%bx
	ret
9:	pop	%dx
8:	xor	$0x80,%dl		# try cycling drive a/c
	xor	%ax,%ax			# reset disk
	int	$0x13
	jc	8b
	jmp	1b
	.endfn	dsknfo

/	Reads disk sector via BIOS.
/
/	@param	al sector number
/	@param	es destination memory address >> 4
/	@param	cx cylinder number
/	@param	dh head number
/	@param	dl drive number
/	@return	number of sectors actually read
pcread:	push	%ax
	push	%cx
	xchg	%cl,%ch
	ror	%cl
	ror	%cl
	or	%al,%cl
	xor	%bx,%bx			# es:bx is destination addr
	mov	$1,%al			# read only one disk sector
	mov	$2,%ah			# read disk sectors ordinal
	int	$0x13
	pop	%cx
	pop	%ax
	jc	9f
	mov	%es,%si
	add	$512>>4,%si
	mov	%si,%es
	inc	%al
	cmp	XLM(DRIVE_LAST_SECTOR),%al
	jbe	2f
	mov	$1,%al
	inc	%cx
	cmp	XLM(DRIVE_LAST_CYLINDER),%cx
	jbe	2f
	xor	%cx,%cx
	inc	%dh
2:	ret
9:	push	%ax
	xor	%ax,%ax			# try disk reset on error
	int	$0x13
	pop	%ax
	jmp	pcread
	.endfn	pcread

/*───────────────────────────────────────────────────────────────────────────│─╗
│ αcτµαlly pδrταblε εxεcµταblε § partition table                           ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

/	Partition Table.
.Lape.mbrpad:
	.org	0x1b4
	.endobj	.Lape.mbrpad
ape_disk:
	.stub	.Lape.diskid,quad
	.org	0x1be,0x00
	.macro	.partn	x
	.stub	.Lape.part\x\().status,byte	# 0=absent / 0x80=present
	.stub	.Lape.part\x\().first.head,byte	# in low 6 bits
	.stub	.Lape.part\x\().first.cylinder,byte
	.stub	.Lape.part\x\().first.sector,byte
	.stub	.Lape.part\x\().filesystem,byte
	.stub	.Lape.part\x\().last.head,byte
	.stub	.Lape.part\x\().last.cylinder,byte
	.stub	.Lape.part\x\().last.sector,byte
	.stub	.Lape.part\x\().lba,long          # c₀*Cₙ + h₀*Hₙ + s₀*Sₙ
	.stub	.Lape.part\x\().sector.count,long # sectors are 512 bytes
	.endm
	.partn	1
	.partn	2
	.partn	3
	.partn	4
	.org	0x1fe
	.short	BOOTSIG
	.endobj	ape_disk

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

apesh:	.ascii	"'\n#'\"\n"			# sixth edition shebang
	.ascii	"o=\"$(command -v \"$0\")\"\n"
	.ascii	"if [ -d /Applications ]; then\n"
	.ascii	  "dd if=\"$o\""
	.ascii	    " of=\"$o\""
	.ascii	    " bs=8"
	.ascii	    " skip=\""
	.shstub	    .Lape.macho.dd.skip,2
	.ascii	    "\" count=\""
	.shstub	    .Lape.macho.dd.count,2
	.ascii	    "\" conv=notrunc 2>/dev/null\n"
	.ascii	"elif exec 7<> \"$o\"; then\n"
	.ascii	  "printf '"
	.ascii	    "\\177ELF"			# 0x0: ⌂ELF
	.ascii	    "\\2"			#   4: long mode
	.ascii	    "\\1"			#   5: little endian
	.ascii	    "\\1"			#   6: elf v1.o
	.ascii	    "\\011"			#   7: FreeBSD
	.ascii	    "\\0"			#   8: os/abi ver.
	.ascii	    "\\0\\0\\0"			#   9: padding 3/7
	.ascii	    "\\0\\0\\0\\0"		#      padding 4/7
	.ascii	    "\\2\\0"			#  10: εxεcµταblε
	.ascii	    "\\076\\0"			#  12: NexGen32e
	.ascii	    "\\1\\0\\0\\0"		#  14: elf v1.o
	.shstub	    .Lape.elf.entry,8		#  18: e_entry
	.shstub	    .Lape.elf.phoff,8		#  20: e_phoff
	.shstub	    .Lape.elf.shoff,8		#  28: e_shoff
	.ascii	    "\\0\\0\\0\\0"		#  30: e_flags
	.ascii	    "\\100\\0"			#  34: e_ehsize
	.ascii	    "\\070\\0"			#  36: e_phentsize
	.shstub	    .Lape.elf.phnum,2		#  38: e_phnum
	.ascii	    "\\0\\0"			#  3a: e_shentsize
	.shstub	    .Lape.elf.shnum,2		#  3c: e_shnum
	.shstub	    .Lape.elf.shstrndx,2	#  3e: e_shstrndx
	.ascii	    "' >&7\n"
	.ascii	  "exec 7<&-\n"
	.ascii	"fi\n"
	.ascii	"exec \"$0\" \"$@\"\n"		# etxtbsy tail recursion
	.ascii	"R=$?\n"			# architecture optimistic
	.ascii	"\n"
	.ascii	"if [ $R -eq 126 ] && [ \"$(uname -m)\" != x86_64 ]; then\n"
	.ascii	  "if Q=\"$(command -v qemu-x86_64)\"; then\n"
	.ascii	    "exec \"$Q\" \"$0\" \"$@\"\n"
	.ascii	  "else\n"
	.ascii	    "echo error: need qemu-x86_64 >&2\n"
	.ascii	  "fi\n"
	.ascii	"elif [ $R -eq 127 ]; then\n"	# means argv[0] was wrong
	.ascii	"  exec \"$o\" \"$@\"\n"	# so do a path resolution
	.ascii	"fi\n"
	.ascii	"exit $R\n"
	.endobj	apesh

	.section .elf.phdrs,"a",@progbits
	.align	__SIZEOF_POINTER__
	.type	ape.phdrs,@object
	.globl	ape.phdrs
ape.phdrs:
	.long	PT_LOAD				# text segment
	.long	PF_R|PF_X
	.stub	.Lape.rom.offset,quad
	.stub	.Lape.rom.vaddr,quad
	.stub	.Lape.rom.paddr,quad
	.stub	.Lape.rom.filesz,quad
	.stub	.Lape.rom.memsz,quad
	.stub	.Lape.rom.align,quad
	.align	__SIZEOF_POINTER__
	.long	PT_LOAD				# data segment
	.long	PF_R|PF_W
	.stub	.Lape.ram.offset,quad
	.stub	.Lape.ram.vaddr,quad
	.stub	.Lape.ram.paddr,quad
	.stub	.Lape.ram.filesz,quad
	.stub	.Lape.ram.memsz,quad
	.stub	.Lape.ram.align,quad
/	Linux ignores mprotect() and returns 0 without this lool
/	It has nothing to do with the stack, which is still exec
	.align	__SIZEOF_POINTER__
	.long	PT_GNU_STACK			# p_type
	.long	PF_R|PF_W			# p_flags
	.quad	0				# p_offset
	.quad	0				# p_vaddr
	.quad	0				# p_paddr
	.quad	0				# p_filesz
	.quad	0				# p_memsz
	.quad	16				# p_align
	.align	__SIZEOF_POINTER__
	.long	PT_NOTE				# openbsd note
	.long	PF_R
	.stub	.Lape.note.offset,quad
	.stub	.Lape.note.vaddr,quad
	.stub	.Lape.note.paddr,quad
	.stub	.Lape.note.filesz,quad
	.stub	.Lape.note.memsz,quad
	.stub	.Lape.note.align,quad
	.previous

	.section .note.openbsd.ident,"a",@progbits
.Lopenbsd.ident:
	.long	8
	.long	4
	.long	0x1
	.asciz	"OpenBSD"
	.long	0
	.size	.Lopenbsd.ident,.-.Lopenbsd.ident
	.type	.Lopenbsd.ident,@object
	.previous

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

.section .macho,"a",@progbits
.align	__SIZEOF_POINTER__

ape.macho:
	.long	0xFEEDFACE+1
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
	.stub	.Lape.rom.vaddr,quad
	.stub	.Lape.rom.memsz,quad
	.stub	.Lape.rom.offset,quad
	.stub	.Lape.rom.filesz,quad
	.long	PROT_EXEC|PROT_READ|PROT_WRITE	# maxprot
	.long	PROT_EXEC|PROT_READ		# initprot
	.long	1				# segment section count
	.long	0				# flags
210:	.ascin	"__text",16			# section name (.text)
	.ascin	"__TEXT",16
	.stub	.Lape.text.vaddr,quad
	.stub	.Lape.text.memsz,quad
	.stub	.Lape.text.offset,long
	.long	12				# align 2**12 = 4096
	.long	0				# reloc table offset
	.long	0				# relocation count
	.long	MAC_S_ATTR_SOME_INSTRUCTIONS	# section type & attributes
	.long	0,0,0				# reserved
30:	.long	MAC_LC_SEGMENT_64
	.long	40f-30b
	.ascin	"__DATA",16
	.stub	.Lape.ram.vaddr,quad
	.stub	.Lape.ram.memsz,quad
	.stub	.Lape.ram.offset,quad
	.stub	.Lape.ram.filesz,quad
	.long	PROT_EXEC|PROT_READ|PROT_WRITE	# maxprot
	.long	PROT_READ|PROT_WRITE		# initprot
	.long	2				# segment section count
	.long	0				# flags
310:	.ascin	"__data",16			# section name (.data)
	.ascin	"__DATA",16
	.stub	.Lape.data.vaddr,quad
	.stub	.Lape.data.memsz,quad
	.stub	.Lape.data.offset,long
	.long	12				# align 2**12 = 4096
	.long	0				# reloc table offset
	.long	0				# relocation count
	.long	0				# section type & attributes
	.long	0,0,0				# reserved
320:	.ascin	"__bss",16			# section name (.bss)
	.ascin	"__DATA",16
	.stub	.Lape.bss.vaddr,quad		# virtual address
	.stub	.Lape.bss.memsz,quad		# memory size
	.long	0				# file offset
	.long	12				# align 2**12 = 4096
	.long	0				# reloc table offset
	.long	0				# relocation count
	.long	MAC_S_ZEROFILL			# section type & attributes
	.long	0,0,0				# reserved
40:	.long	MAC_LC_UUID
	.long	50f-40b
	.stub	uuid1_,quad
	.stub	uuid2_,quad
50:	.long	MAC_LC_UNIXTHREAD
	.long	60f-50b				# cmdsize
	.long	MAC_THREAD_NEXGEN32E		# flavaflav
	.long	(520f-510f)/4			# count
510:	.quad	0				# rax
	.quad	IMAGE_BASE_VIRTUAL		# rbx
	.quad	0				# rcx
	.quad	0				# rdx
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
	.quad	_start_xnu			# rip
	.quad	0				# rflags
	.quad	0				# cs
	.quad	0				# fs
	.quad	0				# gs
520:
60:

.endobj	ape.macho,globl,hidden
.previous /* .macho */

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

/	      ┌14:Uniprocessor Machine            ┌─────────────────────────┐
/	      │┌13:DLL                            │ PE File Characteristics │
/	      ││┌12:System                        ├─────────────────────────┤
/	      │││┌11:If Net Run From Swap         │ r │ reserved            │
/	      ││││┌10:If Removable Run From Swap  │ d │ deprecated          │
/	      │││││┌9:Debug Stripped              │ D │ deprecated with     │
/	      ││││││┌8:32bit Machine              │   │ extreme prejudice   │
/	      │││││││  ┌5:Large Address Aware     └───┴─────────────────────┘
/	      │││││││  │   ┌1:Executable
/	      │││││││  │   │┌0:Relocs Stripped
/	     d│││││││dr│Ddd││
.LPEEXE =  0b0000001000100011

/	     ┌15:TERMINAL_SERVER_AWARE            ┌─────────────────────────┐
/	     │┌14:GUARD_CF                        │ PE DLL Characteristics  │
/	     ││┌13:WDM_DRIVER                     ├─────────────────────────┤
/	     │││┌12:APPCONTAINER                  │ r │ reserved            │
/	     ││││┌11:NO_BIND                      └───┴─────────────────────┘
/	     │││││┌10:NO_SEH
/	     ││││││┌9:NO_ISOLATION
/	     │││││││┌8:NX_COMPAT
/	     ││││││││┌7:FORCE_INTEGRITY
/	     │││││││││┌6:DYNAMIC_BASE
/	     ││││││││││┌5:HIGH_ENTROPY_VA
/	     │││││││││││rrrrr
.LDLLSTD = 0b0000000100100000
.LDLLPIE = 0b0000000001000000
.LDLLEXE = .LDLLSTD

/	     ┌31:Writeable                        ┌─────────────────────────┐
/	     │┌30:Readable                        │ PE Section Flags        │
/	     ││┌29:Executable                     ├─────────────────────────┤
/	     │││┌28:Shareable                     │ o │ for object files    │
/	     ││││┌27:Unpageable                   │ r │ reserved            │
/	     │││││┌26:Uncacheable                 └───┴─────────────────────┘
/	     ││││││┌25:Discardable
/	     │││││││┌24:Contains Extended Relocations
/	     ││││││││        ┌15:Contains Global Pointer (GP) Relative Data
/	     ││││││││        │       ┌7:Contains Uninitialized Data
/	     ││││││││        │       │┌6:Contains Initialized Data
/	     ││││││││ o      │       ││┌5:Contains Code
/	     ││││││││┌┴─┐rrrr│  ooror│││rorrr
.LPETEXT = 0b01110000000000000000000001100000
.LPEDATA = 0b11000000000000000000000011000000
.LPEIMPS = 0b11000000000000000000000001000000

	.section .pe.header,"a",@progbits
	.align	__SIZEOF_POINTER__
ape.pe:	.ascin	"PE",4
	.short	kNtImageFileMachineNexgen32e
	.stub	.Lape.pe.shnum,short	# NumberOfSections
	.long	0x5c64126b		# TimeDateStamp
	.long	0			# PointerToSymbolTable
	.long	0			# NumberOfSymbols
	.stub	.Lape.pe.optsz,short	# SizeOfOptionalHeader
	.short	.LPEEXE			# Characteristics
	.short	kNtPe64bit		# Optional Header Magic
	.byte	14			# MajorLinkerVersion
	.byte	15			# MinorLinkerVersion
	.long	0			# SizeOfCode
	.long	0			# SizeOfInitializedData
	.long	0			# SizeOfUninitializedData
	.long	RVA(WinMain)		# EntryPoint
	.long	0			# BaseOfCode
	.quad	IMAGE_BASE_VIRTUAL	# ImageBase
	.long	4096			# SectionAlignment
	.long	4096			# FileAlignment
	.short	6			# MajorOperatingSystemVersion
	.short	0			# MinorOperatingSystemVersion
	.short	0			# MajorImageVersion
	.short	0			# MinorImageVersion
	.short	6			# MajorSubsystemVersion
	.short	0			# MinorSubsystemVersion
	.long	0			# Win32VersionValue
	.long	RVA(_end)		# SizeOfImage
	.long	RVA(_ehead)		# SizeOfHeaders
	.long	0			# Checksum
	.short	v_ntsubsystem		# Subsystem: 0=Neutral,2=GUI,3=Console
	.short	.LDLLEXE		# DllCharacteristics
	.quad	0x0000000000020000	# StackReserve
	.quad	0x0000000000020000	# StackCommit
	.quad	0			# HeapReserve
	.quad	0			# HeapCommit
	.long	0			# LoaderFlags
	.long	16			# NumberOfDirectoryEntries
	.long	0,0			# ExportsDirectory
	.long	RVA(idata.idt)		# ImportsDirectory
	.stub	.Lidata.idtsize,long	# ImportsDirectorySize
	.long	0,0			# ResourcesDirectory
	.long	0,0			# ExceptionsDirectory
	.long	0,0			# SecurityDirectory
	.long	0,0			# BaseRelocationTable
	.long	0,0			# DebugDirectory
	.long	0,0			# DescriptionString
	.long	0,0			# MachineSpecific
	.long	0,0			# ThreadLocalStorage
	.long	0,0			# LoadConfigurationDirectory
	.long	0,0			# BoundImportDirectory
	.long	RVA(idata.iat)		# ImportAddressDirectory
	.stub	.Lidata.iatsize,long	# ImportAddressDirectorySize
	.long	0,0			# DelayImportDescriptor
	.long	0,0			# ComPlusRuntimeHeader
	.long	0,0			# Reserved
	.endobj	ape.pe,globl
	.previous

	.section .pe.sections,"a",@progbits
	.ascin	".text",8		# Section Name
	.stub	.Lape.text.memsz,long	# Virtual Size or Physical Address
	.stub	.Lape.text.rva,long	# Relative Virtual Address
	.stub	.Lape.text.filesz,long	# Physical Size
	.stub	.Lape.text.offset,long	# Physical Offset
	.long	0			# Relocation Table Offset
	.long	0			# Line Number Table Offset
	.short	0			# Relocation Count
	.short	0			# Line Number Count
	.long	.LPETEXT		# Flags
	.previous

	.section .pe.sections,"a",@progbits
	.ascin	".data",8		# Section Name
	.stub	.Lape.ram.memsz,long	# Virtual Size or Physical Address
	.stub	.Lape.ram.rva,long	# Relative Virtual Address
	.stub	.Lape.ram.filesz,long	# Physical Size
	.stub	.Lape.ram.offset,long	# Physical Offset
	.long	0			# Relocation Table Offset
	.long	0			# Line Number Table Offset
	.short	0			# Relocation Count
	.short	0			# Line Number Count
	.long	.LPEDATA		# Flags
	.previous

	.section .idata.ro.idt.1,"a",@progbits
	.type	idata.idtend,@object
	.type	idata.idt,@object
	.globl	idata.idt,idata.idtend
	.hidden	idata.idt,idata.idtend
idata.idt:
	.previous/*
	...
	decentralized content
	...
	*/.section .idata.ro.idt.3,"a",@progbits
	.long	0,0,0,0,0
idata.idtend:
	.previous

	.section .piro.data.sort.iat.1,"aw",@progbits
	.type	idata.iatend,@object
	.type	idata.iat,@object
	.globl	idata.iat,idata.iatend
	.hidden	idata.iat,idata.iatend
idata.iat:
	.previous/*
	...
	decentralized content
	...
	*/.section .piro.data.sort.iat.3,"aw",@progbits
idata.iatend:
	.previous

/*─────────────────────────────────────────────────────────────────────────────╗
│ αcτµαlly pδrταblε εxεcµταblε § early-stage read-only data                    │
╚──────────────────────────────────────────────────────────────────────────────╝
	better code/data separation (.head is rwx[real] rx[long]) */

/	NUL-Terminated Strings.
ape.str:
.Lstr.ape:
	.byte	0xe0,0x63,0xe7,0xe6,0xe0,0x6c,0x6c,0x79	#αcτµαlly
	.byte	0x20,0x70,0xeb,0x72,0xe7,0xe0,0x62,0x6c	# pδrταbl
	.byte	0xee,0x20,0xee,0x78,0xee,0x63,0xe6,0xe7	#ε εxεcµτ
	.byte	0xe0,0x62,0x6c,0xee,0x0d,0x0a,0x00	#αblε.
	.endobj	.Lstr.ape
.Lstr.error:
	.asciz	"error: "
	.endobj	.Lstr.error
.Lstr.crlf:
	.asciz	"\r\n"
	.endobj	.Lstr.crlf
.Lstr.cpuid:
	.asciz	"cpuid"
	.endobj	.Lstr.cpuid
.Lstr.oldskool:
	.asciz	"oldskool"
	.endobj	.Lstr.oldskool
.Lstr.dsknfo:
	.asciz	"dsknfo"
	.endobj	.Lstr.dsknfo
.Lstr.e820:
	.asciz	"e820"
	.endobj	.Lstr.e820
.Lstr.memory:
	.asciz	"nomem"
	.endobj	.Lstr.memory
.Lstr.long:
	.asciz	"nolong"
	.endobj	.Lstr.long
	.endobj	ape.str

/	Serial Line Configuration (8250 UART 16550)
/	If it's hacked, it'll at least get hacked very slowly.
sconf:	.short	1843200/*hz*/ / 16/*wut*/ / 9600/*baud*/
/
/	          ┌interrupt trigger level {1,4,8,14}
/	          │ ┌enable 64 byte fifo (UART 16750+)
/	          │ │ ┌select dma mode
/	          │ │ │┌clear transmit fifo
/	          │ │ ││┌clear receive fifo
/	          ├┐│ │││┌enable fifos
	.byte	0b00000000
/
/	          ┌dlab: flips configuration mode state
/	          │┌enable break signal
/	          ││ ┌parity {none,odd,even,high,low}
/	          ││ │ ┌extra stop bit
/	          ││ │ │┌data word length (bits+5)
/	          ││┌┴┐│├┐
	.byte	0b01000011
	.endobj	sconf,global,hidden

/	Global Descriptor Table
/
/	@note address portion only concern legacy modes
	.align	8
gdt:	.short	2f-1f		# table byte length
	.long	REAL(1f),0	# table address
	.zero	2
1:
/	          ┌G:granularity (1 → limit *= 0x1000)
/	          │┌D/B:default operation size (0 = 16|64bit, 1 = 32-bit)
/	          ││┌L:long mode
/	          │││┌AVL:this bit is thine (1<<52)
/	          ││││    ┌P:present
/	          ││││    │┌DPL:privilege
/	          ││││    ││ ┌─────────data/code(1)
/	          ││││    ││ │┌────data(0)──────code(1)
/	          ││││    ││ ││┌───conforming───expand-down
/	          ││││    ││ │││┌──writeable────readable
/	          ││││    ││ ││││┌─accessed─────accessed
/	          ││││    ││ │││││
/	          ││││ ┌──││─│││││───────────────────────────────┐
/	      ┌───││││─│──││─│││││───────────┐                   │
/	  ┌───┴──┐││││┌┴─┐│├┐│││││┌──────────┴───────────┐┌──────┴───────┐
/	  │      ││││││  ││││││││││          base address││ segment limit│
/	  │      ││││││  ││││││││││               32 bits││       20 bits│
/	  │      ││││││  ││││││││││                      ││              │
/	  6666555555555544444444443333333333222222222211111111110000000000
/	  3210987654321098765432109876543210987654321098765432109876543210
/	  │      ││││││  ││││││││││                      ││              │
.quad	0b0000000000000000000000000000000000000000000000000000000000000000 # 0
.quad	0b0000000000001111100110100000000000000000000000001111111111111111 # 8
.quad	0b0000000000001111100100100000000000000000000000001111111111111111 #16
.quad	0b0000000011001111100110100000000000000000000000001111111111111111 #24
.quad	0b0000000011001111100100100000000000000000000000001111111111111111 #32
.quad	0b0000000010101111100110110000000000000000000000001111111111111111 #40
.quad	0b0000000010101111100100110000000000000000000000001111111111111111 #48
2:	.endobj	gdt,global,hidden

/*─────────────────────────────────────────────────────────────────────────────╗
│ αcτµαlly pδrταblε εxεcµταblε § multiboot stub                                │
╚──────────────────────────────────────────────────────────────────────────────╝
	boot modernized for the nineties */

#define GRUB_MAGIC 0x1BADB002
#define GRUB_EAX 0x2BADB002
#define GRUB_AOUT (1 << 16)
#define GRUB_CHECKSUM(FLAGS) (-(GRUB_MAGIC + (FLAGS)) & 0xffffffff)

/	Grub Header.
	.align	4
ape.grub:
	.long	GRUB_MAGIC			# Magic
	.long	GRUB_AOUT			# Flags
	.long	GRUB_CHECKSUM(GRUB_AOUT)	# Checksum
	.long	RVA(ape.grub)			# HeaderPhysicalAddress
	.long	IMAGE_BASE_PHYSICAL		# TextPhysicalAddress
	.long	PHYSICAL(_edata)		# LoadEndPhysicalAddress
	.long	PHYSICAL(_end)			# BssEndPhysicalAddress
	.long	RVA(ape.grub.entry)		# EntryPhysicalAddress
	.endobj	ape.grub,globl,hidden

/	Grub Entrypoint.
/	Takes CPU out of legacy mode and jumps to normal entrypoint.
/	@noreturn
	.align	4
ape.grub.entry:
	.code32
	cmp	$GRUB_EAX,%eax
	jne	triplf
	push	$0
	popf
	mov	$0x40,%dl
	mov	%cr0,%eax
	and	$~CR0_PE,%eax
	mov	%eax,%cr0
	ljmpw	$0,$REAL(pc)
	.code16
	.endfn	ape.grub.entry

/*─────────────────────────────────────────────────────────────────────────────╗
│ αcτµαlly pδrταblε εxεcµταblε § real mode                                     │
╚──────────────────────────────────────────────────────────────────────────────╝
	the default mode of operation on modern cpus */

	nop
	nop
	nop
	nop
realmodeloader:
	call	rlinit
	call	sinit4
	mov	$REAL(.Lstr.ape),%di
	call	rvputs
	.optfn	_start16
	call	_start16
	call	longmodeloader
	.endfn	realmodeloader,globl,hidden

	.section .sort.text.real.init.1,"ax",@progbits
	.type	rlinit,@function
rlinit:	.previous/*
	...
	decentralized function
	...
	*/.section .sort.text.real.init.3,"ax",@progbits
	ret
	.previous

/	Initializes present PC serial lines.
sinit4:	mov	$4,%cx
	mov	$kBiosDataAreaXlm+COM1,%si
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

/	Initializes Serial Line Communications 8250 UART 16550A
/
/	@param	word di tty port
/	@param	char (*{es:,e,r}si)[4] register initial values
/	@mode	long,legacy,real
/	@see	www.lammertbies.nl/comm/info/serial-uart.html
sinit:	mov	%di,%dx
	test	%dx,%dx
	jz	2f
	push	%dx
	push	%si
	xorw	%cx,%cx
	mov	$UART_LCR,%cl
	add	%cx,%dx
	lodsb	%ds:(%si),%al
	pop	%si
	or	$UART_DLAB,%al
	out	%al,%dx
	pop	%dx
1:	lodsb	%ds:(%si),%al
	out	%al,%dx
	add	$1,%dx
	sub	$1,%cx
	jns	1b
2:	ret
	.endfn	sinit,global,hidden

/	Video put string.
/
/	@param	di is the string
/	@mode	real
rvputs:	push	%bp
	push	%bx
	mov	%di,%si
0:	lodsb
	test	%al,%al
	je	1f
	mov	$7,%bx				# normal mda/cga style page zero
	mov	$0x0e,%ah			# teletype output al cp437
	int	$0x10				# vidya service
	jmp	0b
1:	pop	%bx
	pop	%bp
	ret
	.endfn	rvputs,globl,hidden

/	Abnormally halts startup.
/
/	@param	di message
/	@mode	real
/	@noreturn
rldie:	push	%di
	mov	$REAL(.Lstr.error),%di
	call	rvputs
	pop	%di
	call	rvputs
	mov	$REAL(.Lstr.crlf),%di
	call	rvputs
	xor	%ax,%ax				# get keystroke
	int	$0x16				# keyboard service
	jmp	triplf
	.endfn	rldie,globl,hidden

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
	mov	$XLM(E820),%di
	mov	$XLM_E820_SIZE,%si
	call	e820
	jc	9f
	call	unreal
/	call	hiload
	call	pinit
	jmp	golong
9:	mov	$REAL(.Lstr.e820),%ax
	call	rldie
	.endfn	longmodeloader,globl,hidden

/	Long Mode Hardware Check
lcheck:	pushf				# check for i8086 / i8088 / i80186
	pop	%ax
	test	$0x80,%ah		# see intel manual volume 1 20.1.2
	jnz	9f			# we now assume 32bit is supported
	pushfl				# now check for i386 or early i486
	pop	%eax			# test ability to change cpuid bit
	mov	%eax,%ecx
	mov	$1<<21,%ebx
	xor	%ebx,%eax
	push	%eax
	popfl
	pushfl
	pop	%eax
	cmp	%eax,%ecx
	je	12f			# we assume cpuid inst is available
	or	%ebx,%eax		# puts cpuid bit in the on position
	push	%eax
	popfl
	mov	$0x80000000,%edi	# get amd ext cpuid thingy length
	mov	%edi,%eax
	inc	%edi
	cpuid				# clobbers eax, ebx, ecx, and edx
	cmp	%edi,%eax
	jl	10f
	mov	%edi,%eax
	cpuid
	mov	$1<<29,%edi		# need nexgen32e long mode support
	and	%edi,%edx
	cmp	%edi,%edx
	jne	10f
	xor	%ax,%ax
1:	ret
9:	mov	$REAL(.Lstr.oldskool),%ax
	jmp	20f
10:	mov	$REAL(.Lstr.long),%ax
	jmp	20f
12:	mov	$REAL(.Lstr.cpuid),%ax
	jmp	20f
20:	call	rldie
	.endfn	lcheck

/	Gets memory map from BIOS.
/
/	@param	di paragraph aligned buffer
/	@param	si bytes in buffer to fill
/	@return	number of bytes written or CF on error
/	@mode	real
e820:	push	%bp
	mov	%sp,%bp
	pushl	$'S<<24|'M<<16|'A<<8|'P	# magic @ -4(%bp)
	push	%bx
	shr	$4,%di
	mov	%di,%es
	xor	%edi,%edi		# es:di is destination buffer
	xor	%ebx,%ebx		# ebx is an api state tracker
1:	mov	$0xE820,%eax		# magic
	mov	$8+8+4+4,%ecx		# sizeof(struct SmapEntry)
	mov	-4(%bp),%edx		# magic
	int	$0x15			# ax,bx,cx,dx,di → ax,bx,cx
	jc	9f			# cf = unsupported or abuse
	cmp	-4(%bp),%eax		# more magic means success
	jne	9f
	test	%cx,%cx			# discard empty results
	jz	5f
	cmp	$8+8+4+1,%cx		# discard if ignore flag
	jb	4f
	testb	$1/*ignore*/,8+8+4/*SmapEntry::__acpi3*/(%di)
	jnz	5f
4:	add	$8+8+4+4,%di		# keep entry
5:	test	%ebx,%ebx		# last entry?
	jz	7f
	cmp	%si,%di			# out of buf?
	jb	1b
7:	mov	%di,%ax
8:	pop	%bx
	leave
	ret
9:	stc
	jmp	8b
	.endfn	e820,globl,hidden

/	Unreal Mode.
/	Makes 4gb of real memory accessible via %fs segment.
unreal:	cli
	lgdt	REAL(gdt)
	mov	%cr0,%eax
	or	$CR0_PE,%al
	mov	%eax,%cr0
	jmp	1f
1:	mov	$GDT_LEGACY_DATA,%cx
	mov	%cx,%fs
	and	$~CR0_PE,%al
	mov	%eax,%cr0
	ljmp	$0,$REAL(1f)
1:	sti
	ret
	.endfn	unreal

/	Loads remainder of executable off disk.
hiload:	push	%bx
	mov	$IMAGE_BASE_REAL,%esi		# relocate, again
	mov	$IMAGE_BASE_PHYSICAL,%ebx
	mov	$v_ape_realsectors,%ecx
	shl	$9,%ecx
	or	$-4,%edx
0:	add	$4,%edx
	cmp	%edx,%ecx
	je	1f
	mov	%fs:(%esi,%edx),%eax
	mov	%eax,%fs:(%ebx,%edx)
	jmp	0b
1:	lea	(%ebx,%edx),%ebx
	mov	$v_ape_highsectors,%di		# then copy rest off disk
	mov	$REAL_SCRATCH_AREA>>4,%ax	# to real memory buffer
	mov	%ax,%es
	mov	XLM(LOADSTATE)+0,%ax
	mov	XLM(LOADSTATE)+2,%cx
	mov	XLM(LOADSTATE)+4,%dx
0:	test	%di,%di
	jz	9f
	mov	%di,%ax
	push	%bx
	xor	%bx,%bx
	call	pcread
	pop	%bx
	sub	%ax,%di
	push	%cx
	mov	%ax,%cx				# copy real buffer to high
	shl	$9,%cx				# no way bios loaded >64k
	xor	%si,%si
1:	mov	%es:(%si),%eax
	mov	%eax,%fs:(%ebx)
	add	$4,%ebx
	add	$4,%si
	sub	$4,%cx
	jnz	1b
	pop	%cx
	jmp	0b
9:	pop	%bx
	ret
	.endfn	hiload

/	Asks keyboard to grant system 65,519 more bytes of memory.
/
/	Yup.
/
/	@assume	realmode && df=0
/	@clob	ax,di,si,es,flags
/	@mode	real
/	@see	wiki.osdev.org/A20_Line
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
	.endfn	a20,globl,hidden	# obj since waste of objdump space

/	Initializes long mode paging.
/
/	Modern computers access memory via four levels of indirection:
/
/	  register char (*(*(*(*ram)[512])[512])[512])[4096] asm(cr3)
/
/	Your page tables grow down in memory, starting from the real
/	stack segment base. This function only defines enough tables
/	to get us started.
#define	TIP REAL_STACK_FRAME
pinit:	push	%ds
	mov	$(TIP-0x4000)>>4,%ax
	mov	%ax,%ds
	movl	$TIP-0x2000+PAGE_V+PAGE_RW,0x3000		# PML4T→PDPT
	movl	$TIP-0x3000+PAGE_V+PAGE_RW,0x2000		# PDPT→PDT
	movl	$TIP-0x4000+PAGE_V+PAGE_RW,0x1000		# PDT→PD
	mov	$0x100000/0x1000,%cx				# PD→512kb
	mov	$PAGE_V+PAGE_RW,%eax
	xor	%si,%si
0:	mov	%eax,(%si)
	add	$0x1000,%eax
	add	$8,%si
	loop	0b
 	movb	$0,0						# unmap null
	pop	%ds
	movl	$TIP-0x4000,XLM(PAGE_TABLE_STACK_POINTER)	# STACK→XLM
	mov	$TIP-0x1000,%eax				# PML4T→CR3
	mov	%eax,%cr3
	ret
	.endfn	pinit,globl,hidden

/	Switch from Real Mode → Long Mode
/
/	@see	Intel Manual V3A §4.1.2
golong:	cli
	lidt	XLM(BADIDT)
	mov	%cr4,%eax
	or	$CR4_PAE|CR4_PGE|CR4_OSFXSR,%eax
	mov	%eax,%cr4
	movl	$EFER,%ecx
	rdmsr
	or	$EFER_LME|EFER_SCE,%eax
	wrmsr
	lgdt	REAL(gdt)
	mov	%cr0,%eax
	or	$CR0_PE|CR0_PG|CR0_MP,%eax
	and	$~CR0_EM,%eax
	mov	%eax,%cr0
	ljmp	$GDT_LONG_CODE,$REAL(long)
	.endfn	golong

/	Long mode is long.
/	@noreturn
	.code64
long:	push	$GDT_LONG_DATA
	pop	%rax
	mov	%eax,%ds
	mov	%eax,%ss
	mov	%eax,%es
	mov	%eax,%fs
	mov	%eax,%gs
	xor	%ebp,%ebp
	mov	$REAL_STACK_FRAME+FRAMESIZE,%esp
	call	__map_image
	ezlea	metal,ax
	jmp	*%rax
	.endfn	long

/	Long mode in virtual address space.
/	@noreturn
metal:
#if USE_SYMBOL_HACK
	.byte	0x0f,0x1f,0207			# nop rdi binbase
	.long	(IMAGE_BASE_VIRTUAL-IMAGE_BASE_REAL)/512
#endif
	xor	%eax,%eax			# clear bss
	mov	$.Lape.bss.vaddr,%edi
	mov	$.Lape.bss.memsz,%ecx
	rep stosb
	.weak	__hostos
	ezlea	__hostos,ax
	test	%rax,%rax
	jz	1f
	movb	$METAL,(%rax)
1:	push	$0				# auxv
	push	$0
	push	$0				# envp
	push	$0				# auxv
	push	$0				# argc
	xor	%edi,%edi
	jmp	_start
	.endfn	metal

/	Avoid linker script variables appearing as code in objdump.
	.macro	.ldsvar	name:req
	.type	\name,@object
	.weak	\name
	.endm
	.ldsvar	_end
	.ldsvar	_etext
	.ldsvar	v_ape_realsectors
	.ldsvar	v_ape_highsectors
	.ldsvar	idata.ro
	.ldsvar	ape.pad.rodata
	.ldsvar	ape.piro
	.ldsvar	ape.piro.end
	.type	.Lape.macho.end,@object
	.type	.Lape.note,@object
	.type	.Lape.note.end,@object
	.type	.Lape.note.vaddr,@object
	.type	.Lape.pe.sections,@object
	.type	.Lape.pe.sections_end,@object
	.type	.Lape.text.nops,@object
	.type	__test_end,@object

	.section .commentprologue,"a",@progbits
	.type	kLegalNotices,@object
	.hidden	kLegalNotices
kLegalNotices:/*
	...
	decentralized content
	...
	*/.previous
	.section .commentepilogue,"a",@progbits
	.byte	0
	.previous

	.section .ape.pad.head,"a",@progbits
	.type	ape.pad.head,@object
	.hidden	ape.pad.head
ape.pad.head:
	.previous

	.section .ape.pad.text,"a",@progbits
	.type	ape.pad.text,@object
	.hidden	ape.pad.text
ape.pad.text:
	.previous

	.section .ape.pad.privileged,"a",@progbits
	.type	ape.pad.privileged,@object
	.hidden	ape.pad.privileged
ape.pad.privileged:
	.previous

	.section .ape.pad.test,"a",@progbits
	.type	ape.pad.test,@object
	.hidden	ape.pad.test
ape.pad.test:
	.previous

	.section .ape.pad.test,"a",@progbits
	.type	ape.pad.test,@object
	.hidden	ape.pad.test
ape.pad.test:
	.previous

	.section .ape.pad.rodata,"a",@progbits
	.type	ape.pad.rodata,@object
	.hidden	ape.pad.rodata
ape.pad.rodata:
	.previous

	.section .ape.pad.data,"a",@progbits
	.type	ape.pad.data,@object
	.hidden	ape.pad.data
ape.pad.data:
	.previous

	.section .idata.ro,"a",@progbits
	.type	idata.ro,@object
	.hidden	idata.ro
idata.ro:
	.previous

	.section .dataprologue,"aw",@progbits
	.type	__data_start,@object
	.globl	__data_start
	.hidden	__data_start
__data_start:
	.previous

	.type	__piro_start,@object
	.hidden	__piro_start

	.type	__ubsan_data_start,@object
	.type	__ubsan_data_end,@object
	.type	__ubsan_types_start,@object
	.type	__ubsan_types_end,@object

.end
