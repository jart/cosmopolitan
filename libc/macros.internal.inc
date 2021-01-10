/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 sw=8 fenc=utf-8                                     :vi│
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
╚─────────────────────────────────────────────────────────────────────────────*/

/	Shorthand notation for widely-acknowledged sections.
.macro	.rodata
	.section .rodata,"a",@progbits
.endm
.macro	.init
	.section .init,"ax",@progbits
.endm
.macro	.real
	.section .text.real,"ax",@progbits
.endm
.macro	.text.startup
	.section .text.startup,"ax",@progbits
.endm
.macro	.text.exit
	.section .text.exit,"ax",@progbits
.endm
.macro	.firstclass
	.section .text.hot,"ax",@progbits
.endm
.macro	.text.unlikely
	.section .text.unlikely,"ax",@progbits
.endm
.macro	.text.likely
	.section .text.hot,"ax",@progbits
.endm
.macro	.text.modernity
	.section .text.modernity,"ax",@progbits
	.align	16
.endm
.macro	.text.antiquity
	.section .text.antiquity,"ax",@progbits
.endm
.macro	.text.hot
	.section .text.hot,"ax",@progbits
.endm
.macro	.preinit_array
	.section .preinit_array,"a",@init_array
.endm
.macro	.init_array
	.section .init_array,"a",@init_array
.endm
.macro	.text.windows
	.section .text.windows,"ax",@progbits
.endm

/	Mergeable numeric constant sections.
/
/	@note	linker de-dupes item/values across whole compile
/	@note	therefore item/values are reordered w.r.t. link order
/	@note	therefore no section relative addressing
.macro	.rodata.cst4
	.section .rodata.cst4,"aM",@progbits,4
	.align	4
.endm
.macro	.rodata.cst8
	.section .rodata.cst8,"aM",@progbits,8
	.align	8
.endm
.macro	.rodata.cst16
	.section .rodata.cst16,"aM",@progbits,16
	.align	16
.endm
.macro	.rodata.cst32
	.section .rodata.cst32,"aM",@progbits,32
	.align	32
.endm
.macro	.rodata.cst64
	.section .rodata.cst64,"aM",@progbits,64
	.align	64
.endm
.macro	.tdata
	.section .tdata,"awT",@progbits
	.align	4
.endm
.macro	.tbss
	.section .tdata,"awT",@nobits
	.align	4
.endm

/	Mergeable NUL-terminated UTF-8 string constant section.
/
/	@note	linker de-dupes C strings here across whole compile
/	@note	therefore item/values are reordered w.r.t. link order
/	@note	therefore no section relative addressing
.macro	.rodata.str1.1
	.section .rodata.str1.1,"aSM",@progbits,1
	.align	1
.endm

/	Locates unreferenced code invulnerable to --gc-sections.
.macro	.keep.text
	.section .keep.text,"ax",@progbits
.endm

/	Flags code as only allowed for testing purposes.
.macro	.testonly
	.section .test,"ax",@progbits
.endm

/	Makes code runnable while code morphing.
.macro	.privileged
	.section .privileged,"ax",@progbits
.endm

/	Post-Initialization Read-Only (PIRO) BSS section.
/	@param	ss is an optional string, for control image locality
.macro	.piro	ss
 .ifnb	\ss
	.section .piro.sort.bss.\ss,"aw",@nobits
 .else
	.section .piro.bss,"aw",@nobits
 .endif
.endm

/	Helpers for Cosmopolitan _init() amalgamation magic.
/	@param	name should be consistent across macros for a module
/	@see	libc/runtime/_init.S
.macro	.initro number:req name:req
	.section .initro.\number\().\name,"a",@progbits
	.align	8
.endm
.macro	.initbss number:req name:req
	.section .piro.bss.init.2.\number\().\name,"aw",@nobits
	.align	8
.endm
.macro	.init.start number:req name:req
	.section .init.\number\().\name,"ax",@progbits
\name:
.endm
.macro	.init.end number:req name:req bnd=globl vis
	.endfn	\name,\bnd,\vis
	.previous
.endm

/	Declares alternative implementation of function.
/	@param	implement e.g. tinymath_pow
/	@param	canonical e.g. pow
.macro	.alias	implement:req canonical:req
	.equ	\canonical,\implement
	.weak	\canonical
.endm

/	Ends function definition.
/	@cost	saves 1-3 lines of code
.macro	.endfn	name:req bnd vis
 .size	\name,.-\name
 .type	\name,@function
 .ifnb	\bnd
  .\bnd	\name
 .endif
 .ifnb	\vis
  .\vis	\name
 .endif
.endm

/	Ends variable definition.
/	@cost	saves 1-3 lines of code
.macro	.endobj	name:req bnd vis
 .size	\name,.-\name
 .type	\name,@object
 .ifnb	\bnd
  .\bnd	\name
 .endif
 .ifnb	\vis
  .\vis	\name
 .endif
.endm

/	LOOP Instruction Replacement.
/	With its mop-Fusion Mexican equivalent.
/	Thus avoiding 3x legacy pipeline slowdown.
.macro	.loop	label:req
	.byte	0x83,0xe9,0x01			# sub $1,%ecx
	jnz	\label
.endm

/	Pushes CONSTEXPR ∈ [-128,127].
/	@note	assembler is wrong for non-literal constexprs
.macro	pushb	x:req
	.byte	0x6a,\x
.endm

/	Sign-extends CONSTEXPR ∈ [-128,127] to REGISTER.
/	@cost	≥1 cycles, -2 bytes
.macro	pushpop	constexpr:req register:req
	pushb	\constexpr
	pop	\register
.endm

/	Moves REGISTER to REGISTER.
/	@cost	≥1 cycles, -1 REX byte
.macro	movpp	src:req dest:req
	push	\src
	pop	\dest
.endm

/	Declares optional function.
.macro	.optfn	fn:req
	.globl	\fn
	.weak	\fn
	.equ	\fn,missingno
	.type	\fn,@function
.endm

/	Embeds fixed-width zero-filled string table.
/	@note	zero-padded ≠ nul-terminated
.macro	.fxstr	width head rest:vararg
 .ifnb	\head
0:	.ascii	"\head"
	.org	0b+\width
	.fxstr	\width,\rest
 .endif
.endm

/	Embeds Fixed-Width Zero-Padded String.
/	@note	.fxstr is better
.macro	.ascin str:req fieldsize:req
1347:	.ascii	"\str"
 .org	1347b+\fieldsize,0x00
.endm

/	Marks symbols as object en-masse.
/	@note	zero-padded ≠ nul-terminated
.macro	.object	symbol rest:vararg
 .ifnb	\symbol
	.type	\symbol,@object
	.object	\rest
 .endif
.endm

/	Pads function prologue unconditionally for runtime hooking.
/	@cost	≥0.3 cycles, 5 bytes
/	@see	.profilable
.macro	.hookable
/	nopl 0x00(%rax,%rax,1)
83457:	.byte	0x0f,0x1f,0x44,0x00,0x00
 .section __mcount_loc,"a",@progbits
  .quad	83457b
 .previous
.endm

/	Puts initialized data in uninitialized data section.
.macro	.bsdata	name:req expr:req bnd vis
  .section .initbss.300._init_\name,"aw",@nobits
\name:	.quad	0
	.endobj	\name,\bnd,\vis
  .previous
  .section .initro.300._init_\name,"a",@progbits
	.quad	\expr
  .previous
  .section .init.300._init_\name,"ax",@progbits
_init_\name:
	movsq
	.endfn	_init_\name
  .previous
.endm

/	ICE Breakpoint.
/	Modern gas forgot this but objdump knows
/	@mode	long,legacy,real
	.macro	icebp
	.byte	0xF1
	.endm
	.macro	int1
	icebp
	.endm

/	Sets breakpoint for software debugger.
/	@mode	long,legacy,real
.macro	.softicebp
	.byte	0x53		# push bx
	.byte	0x87,0xdb	# xchg bx,bx (bochs breakpoint)
	.byte	0x5b		# pop bx
	.byte	0x66,0x90	# xchg ax,ax (microsoft breakpoint)
	int3			# gdb breakpoint
.endm

/	Assembles Intel Official 4-Byte NOP.
.macro	fatnop4
 .byte	0x0f,0x1f,0x40,0x00
.endm

/	Pulls unrelated module into linkage.
/
/	In order for this technique to work with --gc-sections, another
/	module somewhere might want to weakly reference whats yoinked.
.macro	yoink	symbol:req
	.section .yoink
	nop	"\symbol"
	.previous
.endm
.macro	.yoink	symbol:req
	.section .yoink
	nop	"\symbol"
	.previous
.endm

/	Calls Windows function.
/
/	@param	cx,dx,r8,r9,stack
/	@return	ax
/	@clob	ax,cx,dx,r8-r11
.macro	ntcall	symbol:req
	sub	$32,%rsp
	call	*\symbol(%rip)
	add	$32,%rsp
.endm

/	Custom emulator instruction for bottom stack frame.
.macro	bofram	endfunc:req
	.byte	0x0f,0x1f,0105,\endfunc-.	# nopl disp8(%rbp)
.endm
