/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set noet ft=asm ts=8 sw=8 fenc=utf-8                                 :vi │
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

#define VGA_PREFER_GRAPH_HEIGHT \
	(VGA_PREFER_TTY_HEIGHT * VGA_ASSUME_CHAR_HEIGHT_PX)
#define VGA_PREFER_GRAPH_WIDTH  \
	(VGA_PREFER_TTY_WIDTH * VGA_ASSUME_CHAR_WIDTH_PX)
#define MAX_VESA_MODES_TO_TRACK 64

//	Mode information data structure, used internally.
	.set	"mi::type",0
	.set	"mi::bpp",1
	.set	"mi::width",2
	.set	"mi::height",4
	.set	"mi::mode_num",6
	.set	"mi::stride",8
	.set	"mi::fb",10
	.set	"mi::sizeof",14

//	Routine to activate additional VESA functionality if the user holds
//	down a magic key, when booting from bare metal.
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
	push	%si			# display brief message on magic key
	mov	$REAL(str.inform),%si
	call	.puts
	pop	%si
	mov	$0x8300,%ax		# wait for the magic key for a short
	mov	$(1000000>>16),%cx	# period of time...
	mov	$(1000000&0xffff),%dx
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
	call	.done_inform
	stc
.done:	pop	%ax
	pop	%es
	ret
.doit2:	mov	$0x8301,%ax		# we got the magic key; cancel the
	int	$0x15			# wait timer, & erase message
	call	.done_inform
.doit:	pop	%ax			# we got the magic key; do stuff
	pop	%es
	// fall through
	.endfn	_rlinit_vesa,globl,hidden

.do_vesa_rlinit:
	push	%eax
	push	%bx
	push	%edx
	push	%bp
	sub	$2+MAX_VESA_MODES_TO_TRACK*"mi::sizeof",%sp
	mov	%sp,%bp
	call	.gather_vesa_modes	# gather list of VESA modes
	jc	8f
	call	.get_default_mode	# choose a default mode to use
	jc	6f
	call	.choose_mode		# allow user to select different mode
	movw	%bx,%bp
	call	.snooze
	mov	"mi::mode_num"(%bp),%bx
	mov	$0x4f02,%ax
	int	$0x10
	cmp	$0x004f,%ax
	jnz	9f
	mov	"mi::type"(%bp),%al
	.set	mm,0x0500
	mov	%al,mm+"struct mman::pc_video_type"
	mov	"mi::width"(%bp),%ax
	mov	%ax,mm+"struct mman::pc_video_width"
	movzwl	"mi::height"(%bp),%edx
	mov	%dx,mm+"struct mman::pc_video_height"
	mov	"mi::fb"(%bp),%eax
	mov	%eax,mm+"struct mman::pc_video_framebuffer"
	movzwl	"mi::stride"(%bp),%eax
	mov	%ax,mm+"struct mman::pc_video_stride"
	imul	%edx,%eax
	mov	%eax,mm+"struct mman::pc_video_framebuffer_size"
	xor	%eax,%eax
	mov	%eax,mm+"struct mman::pc_video_framebuffer"+4
	mov	%eax,mm+"struct mman::pc_video_framebuffer_size"+4
	clc
5:	lahf
	add	$2+MAX_VESA_MODES_TO_TRACK*"mi::sizeof",%sp
	sahf
	pop	%bp
	pop	%edx
	pop	%bx
	pop	%eax
	ret
6:	mov	$REAL(str.no_mode),%si
7:	call	.puts
8:	call	.snooze
	stc
	jmp	5b
9:	mov	$REAL(str.bad_mode),%si
	jmp	7b
	.endfn	.do_vesa_rlinit

//	Clear the informational message on the magic key.
.done_inform:
	mov	$0x0a00|' ',%ax
	mov	$7,%bx
	mov	$str.inform.end-str.inform-1,%cx
	int	$0x10
	ret

//	Preliminarily choose a "default" VESA screen mode from a list of
//	gathered screen modes.
//
//	@return	%bx is such that %ss:(%bx) = internal struc. for default mode
//	@return	CF = 1 if no available modes, CF = 0 otherwise
.get_default_mode:
	push	%eax
	push	%cx
	push	%edx
	push	%edi
	push	%bp
	mov	(%bp),%cx
	jcxz	8f
	inc	%bp
	inc	%bp
	or	$-1,%esi		# %esi = best fit screen size + penalty
					# (to be recalculated later)
	mov	%bp,%bx			# %bx = pointer to best info. struct.
1:	mov	"mi::type"(%bp),%ax	# load mode type & bits per pixel
	ror	%ah			# convert bpp into penalty value:
	movzbl	%ah,%edi		# fewer bpp are better, but prefer
					# 16 bpp over 15 bpp
	cmp	$PC_VIDEO_TEXT,%al	# handle text modes specially
	jz	9f
	movzwl	"mi::width"(%bp),%eax
	jz	9f
	cmp	$VGA_PREFER_GRAPH_WIDTH,%ax # calculate screen size
	jb	3f
	movzwl	"mi::height"(%bp),%edx
	cmp	$VGA_PREFER_GRAPH_HEIGHT,%dx
	jb	3f
	imul	%edx,%eax
2:	add	%edi,%eax
	jc	3f
	cmp	%esi,%eax
	jnb	3f
	mov	%eax,%esi
	mov	%bp,%bx
3:	add	$"mi::sizeof",%bp
	loop	1b
	clc
7:	pop	%bp
	pop	%edi
	pop	%edx
	pop	%cx
	pop	%eax
	ret
8:	stc
	jmp	7b
9:	imul	$VGA_ASSUME_CHAR_WIDTH_PX,%eax
	cmp	$VGA_PREFER_GRAPH_WIDTH,%eax
	jb	2b
	movzwl	"mi::height"(%bp),%edx
	imul	$VGA_ASSUME_CHAR_HEIGHT_PX,%edx
	cmp	$VGA_PREFER_GRAPH_HEIGHT,%edx
	jb	3b
	imul	%edx,%eax
	jo	3b
	mov	$1<<31,%edi		# severely disparage text modes
	jmp	2b
	.endfn	.get_default_mode

//	Allow the user to choose a VESA screen mode to use.
//
//	@return	%bx is such that ss:(%bx) = internal struc. for chosen mode
.choose_mode:
	push	%ax
	push	%si
	push	%di
	mov	(%bp),%di
	imul	$"mi::sizeof",%di
	lea	2(%bp,%di),%di
1:	mov	$REAL(str.choose_mode),%si
	call	.puts
	mov	%ss:("mi::mode_num")(%bx),%ax
	call	.putx
	mov	%ss:("mi::width")(%bx),%ax
	call	.putd
	mov	%ss:("mi::height")(%bx),%ax
	call	.putd
	mov	%ss:("mi::bpp")(%bx),%al
	call	.putdb
	mov	$0,%ah
	int	$0x16
#define UPSCN 0x48
#define DNSCN 0x50
#define CRSCN 0x1c
	cmp	$DNSCN,%ah
	jz	3f
	cmp	$CRSCN,%ah
	jz	4f
	cmp	$UPSCN,%ah
	jnz	1b
	lea	2(%bp),%ax		# up arrow pressed
	cmp	%ax,%bx
	jz	1b
2:	sub	$"mi::sizeof",%bx
	jmp	1b
3:	add	$"mi::sizeof",%bx	# down arrow pressed
	cmp	%di,%bx
	jnz	1b
	jmp	2b
4:	call	.putnl			# Enter pressed
	pop	%di
	pop	%si
	pop	%ax
	ret
	.endfn	.choose_mode

.snooze:
	push	%ax
	push	%cx
	push	%dx
	mov	$0x86,%ah		# do a(nother) short few-second wait
	mov	$(2000000>>16),%cx
	mov	$(2000000&0xffff),%dx
	int	$0x15
	pop	%dx
	pop	%cx
	pop	%ax
	ret
	.endfn	.snooze

//	Dump a list of all the VESA VBE video modes that are usable on the
//	target machine.  Also gather a list of video modes and basic
//	information about these modes at %ss:(%bp).
.gather_vesa_modes:
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
	jnz	.fail2
	mov	$REAL(str.mode_list_start),%si
	call	.puts			# otherwise start iterating through
	lds	%es:0xe(%di),%si	# the returned video mode list
	movw	$0,(%bp)
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
	clc
.done2:	add	$0x200,%sp		# OK, we are finally done
	pop	%di
	pop	%si
	pop	%es
	pop	%ds
	ret
.fail2:	mov	$REAL(str.no_vesa),%si
	call	.puts
	stc
	jmp	.done2
	.endfn	.gather_vesa_modes

//	Display information on one video mode number, which should be in %ax.
//	If %ax is a mode which we can use, also update the mode information
//	buffer at %ss:(%bp).  Assume %es = %ss.
//
//	@return	CF = 0 if video mode is usable, CF = 1 otherwise
.munge:	push	%ax
	push	%bx
	push	%cx
	push	%si
	push	%di
	or	$1<<14,%ax		# force frame buffer mode
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
	call	.video_type		# check if we know about the video
	jc	.fail3			# buffer type, & what exact type it is
	mov	(%bp),%bx		# if we are already tracking too many
	cmp	$MAX_VESA_MODES_TO_TRACK,%bx # VESA modes, also skip
	jnb	.fail3
	inc	%bx			# otherwise start noting down mode
	mov	%bx,(%bp)		# information...
	imul	$"mi::sizeof",%bx
	lea	2-"mi::sizeof"(%ebp,%ebx),%bx
	mov	%al,%ss:("mi::type")(%bx) # ...starting from frame buffer type
	call	.putsp			# echo and remember mode information
	call	.putsp
	test	$0b00010000,%cl		# first, echo mode attributes
	setnz	%al
	imul	$'G'-'T',%ax,%ax	# - 'G': graphics; 'T': text mode
	add	$'T',%al
	call	.putc
	call	.putsp
	xchg	%ax,%si			# then process
	mov	%ax,%ss:("mi::mode_num")(%bx) # - mode number
	call	.putx
	mov	%es:0x12(%di),%ax	# - mode width
	mov	%ax,%ss:("mi::width")(%bx)
	call	.putd
	mov	%es:0x14(%di),%ax	# - mode height
	mov	%ax,%ss:("mi::height")(%bx)
	call	.putd
	mov	%es:0x19(%di),%al	# - bits per pixel
	mov	%al,%ss:("mi::bpp")(%bx)
	call	.putdb
	mov	%es:0x10(%di),%ax	# - mode stride
	mov	%ax,%ss:("mi::stride")(%bx)
	testb	$0b00010000,%cl
	jz	.txt
	mov	%es:0x28(%di),%eax
.fb:	mov	%eax,%ss:("mi::fb")(%bx) # - frame buffer address
	clc
.done3:	lea	0x100(%esp),%sp
	pop	%di
	pop	%si
	pop	%cx
	pop	%bx
	pop	%ax
	ret
.fail3:	stc
	jmp	.done3
.txt:	movzwl	%es:8(%di),%eax		# for text mode, use window A as
	shl	$4,%eax			# frame buffer
	jmp	.fb
	.endfn	.munge

//	Check if the given video mode information uses a video buffer type
//	we know about, and say what type of video buffer it is.
//
//	@param	%es:(%di) = video mode information from int 0x10, %ax = 0x4f01
//	@param	%cl = low byte of video mode attributes (= %es:(%di))
//	@return	%al = video buffer type (for mman::pc_video_type)
//	@return	CF = 0 if video mode is usable, CF = 1 otherwise
.video_type:
	push	%bx
	push	%cx
	push	%edx
	mov	$PC_VIDEO_TEXT,%al	# if text mode, simply say so
	test	$0b00010000,%cl
	jz	.ok4
	cmpb	$6,%es:0x1b(%di)	# if graphics mode, check if direct
	jnz	.fail4			# color; bail out if not
	mov	%es:0x19(%di),%cl	# check actual color fields & bits
	mov	%es:0x1f(%di),%ax	# per pixel, against a list
	mov	%es:0x21(%di),%edx
	mov	$REAL(.type_list),%bx
.iter4:	cmp	%edx,%cs:4(%bx)
	jnz	.next
	cmp	%cl,%cs:1(%bx)
	jnz	.next
	cmp	%ax,%cs:2(%bx)
	jz	.found
.next:	add	$8,%bx
	cmp	$REAL(.type_list_end),%bx
	jnz	.iter4
.fail4:	stc				# unsupported mode; return failure
	jmp	.done4
.found:	mov	%cs:(%bx),%al		# this mode is supported; return the
.ok4:	clc				# corresponding type code
.done4:	pop	%edx
	pop	%cx
	pop	%bx
	ret

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

str.inform:
#define SHGLYPH	"\x7f"
	.ascii	"\rinfo: press ",SHGLYPH,"Shift "
	.asciz	"to switch video mode\r"
str.inform.end:
	.endobj	str.inform
str.no_vesa:
	.asciz	"info: no VESA\r\n"
	.endobj	str.no_vesa
str.choose_mode:
#define UPGLYPH "\x18"
#define DNGLYPH "\x19"
#define CRGLYPH "\x1b\xd9"
	.ascii	"\rchoose video mode (",UPGLYPH," ",DNGLYPH," ",CRGLYPH
	.asciz	"): "
	.endobj	str.choose_mode
str.no_mode:
	.asciz	"info: no usable video mode\r\n"
	.endobj	str.no_mode
str.bad_mode:
	.asciz	"info: mode switch fail\r\n"
	.endobj	str.bad_mode
str.mode_list_start:
	.ascii	"info: VESA video modes:\r\n"
	.ascii	"     mode#     X     Y bpp"
	.ascii	"     mode#     X     Y bpp"
	.asciz	"     mode#     X     Y bpp\r\n"
	.endobj	str.mode_list_start

.type_list:
//		              ┌value to use for mman::pc_video_type
//		              │    ┌bits per pixel
//		              │    │  ┌red field size (in bits)
//		              │    │  │  ┌red field position (bits)
//		              │    │  │  │  ┌green field size
//		              │    │  │  │  │  ┌green field position
//		              │    │  │  │  │  │  ┌blue field size
//		              │    │  │  │  │  │  │  ┌blue field position
//		              │    │  │  │  │  │  │  │
	.byte	PC_VIDEO_BGR565,  16, 5,11, 6, 5, 5, 0
	.byte	PC_VIDEO_BGR555,  15, 5,10, 5, 5, 5, 0
	.byte	PC_VIDEO_BGR555,  16, 5,10, 5, 5, 5, 0
	.byte	PC_VIDEO_RGBX8888,32, 8, 0, 8, 8, 8,16
	.byte	PC_VIDEO_BGRX8888,32, 8,16, 8, 8, 8, 0
	.endobj	.type_list
.type_list_end:

	.previous
