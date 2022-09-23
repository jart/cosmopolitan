/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ This is free and unencumbered software released into the public domain.      │
│                                                                              │
│ Anyone is free to copy, modify, publish, use, compile, sell, or              │
│ distribute this software, either in source code form or as a compiled        │
│ binary, for any purpose, commercial or non-commercial, and by any            │
│ means.                                                                       │
│                                                                              │
│ In jurisdictions that recognize copyright laws, the author or authors        │
│ of this software dedicate any and all copyright interest in the              │
│ software to the public domain. We make this dedication for the benefit       │
│ of the public at large and to the detriment of our heirs and                 │
│ successors. We intend this dedication to be an overt act of                  │
│ relinquishment in perpetuity of all present and future rights to this        │
│ software under copyright law.                                                │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,              │
│ EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF           │
│ MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.       │
│ IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR            │
│ OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        │
│ ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR        │
│ OTHER DEALINGS IN THE SOFTWARE.                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "ape/relocations.h"
#include "libc/macros.internal.h"
#include "libc/runtime/mman.internal.h"
#include "libc/vga/vga.internal.h"

//	Routine to activate additional VESA functionality if the user holds
//	down a magic key, when booting from bare metal.  Currently this just
//	dumps a list of all usable VESA VBE video modes on the console.
//
//	TODO: allow user to select a video mode and switch to it
//	TODO: implement character drawing in graphics modes
//
//	@return	CF = 0 if we decided to set a new video mode, CF = 1 otherwise

	.real
	.code16
_rlinit_vesa:
	push	%es
	pushw	$0
	testb	$0b00000011,0x0417	# read keyboard shift state (as
	jnz	.doit			# given by BIOS's IRQ 1); if Shift
					# key pressed, activate code below
	mov	$0x8300,%ax		# wait for the magic key for a short
	mov	$(250000>>16),%cx	# period of time...
	mov	$(250000&0xffff),%dx
	push	%ss
	pop	%es
	mov	%sp,%bx
	int	$0x15
	jc	.done
.wait:	pause
	testb	$0b00000011,0x0417
	jnz	.doit2
	cmpb	$0,%es:(%bx)
	jz	.wait
.done:	pop	%ax
	pop	%es
	stc
	ret
.doit2:	mov	$0x8301,%ax		# we got the magic key; cancel the
	int	$0x15			# wait timer
.doit:	call	.dump_vesa_modes	# we got the magic key; do stuff
	push	%si
	mov	$REAL(str.cont),%si	# say we are continuing
	call	.puts
	pop	%si
	mov	$0x86,%ah		# do a(nother) short few-second wait
	mov	$(2000000>>16),%cx
	mov	$(2000000&0xffff),%dx
	int	$0x15
	jmp	.done
	.endfn	_rlinit_vesa,globl,hidden

//	Dump a list of all the VESA VBE video modes that are usable on the
//	target machine.
.dump_vesa_modes:
	push	%ds
	push	%es
	push	%si
	push	%di
	push	%ss			# allocate 0x200 bytes on stack
	pop	%es			# for general VESA information
	sub	$0x200,%sp
	mov	$0x200/2,%cx
	mov	%sp,%di
	cld
	xor	%ax,%ax
	rep stosw
	mov	$0x4f00,%ax		# get general VESA information
	mov	%sp,%di
	movl	$/*"VBE2"*/0x32454256,%es:(%di)
	int	$0x10
	cmp	$0x004f,%ax		# if this fails, complain
	jnz	.fail
	mov	$REAL(str.mode_list_start),%si
	call	.puts			# otherwise start iterating through
	lds	%es:0xe(%di),%si	# the returned video mode list
	cld
.iter1:	lodsw
	inc	%ax
	jz	.done2
	dec	%ax
	call	.munge			# process mode number
	jc	.iter1
.iter2:	lodsw
	inc	%ax
	jz	.nl
	dec	%ax
	call	.munge			# process another mode number
	jc	.iter2
.iter3:	lodsw
	inc	%ax
	jz	.nl
	dec	%ax
	call	.munge			# process another mode number
	jc	.iter3
	call	.putnl
	jmp	.iter1			# ...and so on
.nl:	call	.putnl
.done2:	add	$0x200,%sp		# OK, we are finally done
	pop	%di
	pop	%si
	pop	%es
	pop	%ds
	ret
.fail:	mov	$REAL(str.no_vesa),%si
	call	.puts
	jmp	.done2
	.endfn	.dump_vesa_modes

//	Process one video mode number, which should be in ax.  Assume that
//	es = ss.
//
//	@return	CF = 0 if video mode is usable, CF = 1 otherwise
.munge:	push	%ax
	push	%cx
	push	%si
	push	%di
	xchg	%ax,%si			# remember mode number
	sub	$0x100,%sp		# allocate 0x100 stack bytes for
	mov	$0x100/2,%cx		# information on this mode; clear
	mov	%sp,%di			# the bytes
	cld
	xor	%ax,%ax
	rep stosw
	mov	%si,%cx			# get information on one mode
	mov	$0x4f01,%ax
	mov	%sp,%di
	int	$0x10
	cmp	$0x004f,%ax		# if error, skip mode
	jnz	.fail3
	mov	%es:(%di),%al
	mov	%al,%cl
	and	$0b00001011,%al		# also skip if mode is unusable, or
	cmp	$0b00001011,%al		# extra information unavailable, or
	jnz	.fail3			# is monochrome, or is graphics w/o
	mov	%cl,%al			# linear frame buffer
	and	$0b10010000,%al
	cmp	$0b00010000,%al
	jz	.fail3
	call	.putsp			# echo mode information
	call	.putsp
	test	$0b00010000,%cl		# first, echo mode attributes
	setnz	%al
	imul	$'G'-'T',%ax,%ax	# - 'G': graphics; 'T': text mode
	add	$'T',%al
	call	.putc
	call	.putsp
	xchg	%ax,%si			# then output
	call	.putx			# - mode number
	mov	%es:0x12(%di),%ax	# - mode width
	call	.putd
	mov	%es:0x14(%di),%ax	# - mode height
	call	.putd
	mov	%es:0x19(%di),%al	# - bits per pixel
	call	.putdb
	clc
.done3:	lea	0x100(%esp),%sp
	pop	%di
	pop	%si
	pop	%cx
	pop	%ax
	ret
.fail3:	stc
	jmp	.done3
	.endfn	.munge

//	Output a string via BIOS.
.puts:	cld
	push	%si
0:	lodsb
	test	%al,%al
	jz	1f
	call	.putc
	jmp	0b
1:	pop	%si
	ret
	.endfn	.puts

//	Output a 16-bit number in decimal via BIOS.
.putd:	push	%ax
	push	%cx
	push	%dx
	push	%si
	push	%ds
	push	%ss
	pop	%ds
	sub	$8,%sp
	lea	4(%esp),%si
	mov	$10,%cx
	xor	%dx,%dx
	div	%cx
	add	$'0'|' '<<8,%dx
	mov	%dx,(%si)
	movb	$0,2(%si)
1:	mov	$' ',%dl
	test	%ax,%ax
	jz	2f
	xor	%dx,%dx
	div	%cx
	add	$'0',%dl
2:	dec	%si
	mov	%dl,(%si)
	cmp	%sp,%si
	jnz	1b
	call	.puts
	add	$8,%sp
	pop	%ds
	pop	%si
	pop	%dx
	pop	%cx
	pop	%ax
	ret
	.endfn	.putd

//	Output an 8-bit number in decimal via BIOS.
.putdb:	push	%ax
	mov	%al,%ah
	cmp	$100,%al
	jnb	3f
	mov	$' ',%al
1:	call	.putc
	mov	%ah,%al
	aam
	testb	%ah,%ah
	jz	5f
	add	$'0'|'0'<<8,%ax
2:	xchg	%al,%ah
	call	.putc
	mov	%ah,%al
	call	.putc
	pop	%ax
	ret
3:	cmp	$200,%al
	jnb	4f
	sub	$100,%ah
	mov	$'1',%al
	jmp	1b
4:	sub	$200,%ah
	mov	$'2',%al
	jmp	1b
5:	add	$'0'|' '<<8,%ax
	jmp	2b

//	Output a number in hexadecimal via BIOS.
.putx:	push	%ax
	push	%bx
	push	%cx
	xchg	%ax,%bx
	mov	$'0',%al
	call	.putc
	mov	$'x',%al
	call	.putc
	mov	$4,%cx
0:	rol	$4,%bx
	mov	%bl,%al
	and	$0b00001111,%al
	add	$'0',%al
	cmp	$'9',%al
	jna	1f
	add	$'a'-'9'-1,%al
1:	call	.putc
	loop	0b
	pop	%cx
	pop	%bx
	pop	%ax
	.endfn	.putx
	// fall through

//	Output a character via BIOS.
.putsp:	mov	$' ',%al
	.endfn	.putsp
	// fall through

.putc:	push	%ax
	push	%bx
	mov	$7,%bx
	mov	$0x0e,%ah
	int	$0x10
	pop	%bx
	pop	%ax
	ret
	.endfn	.putc

.putnl:
	mov	$'\r',%al
	call	.putc
	mov	$'\n',%al
	jmp	.putc
	.endfn	.putnl
	.previous

str.no_vesa:
	.asciz	"info: no VESA\r\n"
	.endobj	str.no_vesa
str.mode_list_start:
	.ascii	"info: VESA video modes:\r\n"
	.ascii	"     mode#   wid    ht bpp"
	.ascii	"     mode#   wid    ht bpp"
	.asciz	"     mode#   wid    ht bpp\r\n"
	.endobj	str.mode_list_start
str.cont:
	.asciz	"info: continuing\r\n"
	.endobj	str.cont
