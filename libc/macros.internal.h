#ifndef COSMOPOLITAN_LIBC_MACROS_H_
#define COSMOPOLITAN_LIBC_MACROS_H_
#if 0
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § macros                                                    ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/
#endif

/**
 * @fileoverview Common C preprocessor, assembler, and linker macros.
 */

#define TRUE  1
#define FALSE 0

#define IS2POW(X)           (!((X) & ((X)-1)))
#define ROUNDUP(X, K)       (((X) + (K)-1) & -(K))
#define ROUNDDOWN(X, K)     ((X) & -(K))
#ifndef __ASSEMBLER__
#define ABS(X)              ((X) >= 0 ? (X) : -(X))
#define MIN(X, Y)           ((Y) > (X) ? (X) : (Y))
#define MAX(X, Y)           ((Y) < (X) ? (X) : (Y))
#else
// The GNU assembler does not grok the ?: ternary operator; furthermore,
// boolean expressions yield -1 and 0 for "true" and "false", not 1 and 0.
#define __MAPBOOL(P)        (!!(P) / (!!(P) + !(P)))
#define __IFELSE(P, X, Y)   (__MAPBOOL(P) * (X) + __MAPBOOL(!(P)) * (Y))
#define MIN(X, Y)           (__IFELSE((Y) > (X), (X), (Y)))
#define MAX(X, Y)           (__IFELSE((Y) < (X), (X), (Y)))
#endif
#define PASTE(A, B)         __PASTE(A, B)
#define STRINGIFY(A)        __STRINGIFY(A)
#define EQUIVALENT(X, Y)    (__builtin_constant_p((X) == (Y)) && ((X) == (Y)))
#define TYPE_BIT(type)      (sizeof(type) * CHAR_BIT)
#define TYPE_SIGNED(type)   (((type)-1) < 0)
#define TYPE_INTEGRAL(type) (((type)0.5) != 0.5)

#define ARRAYLEN(A) \
  ((sizeof(A) / sizeof(*(A))) / ((unsigned)!(sizeof(A) % sizeof(*(A)))))

#define __STRINGIFY(A) #A
#define __PASTE(A, B)  A##B
#ifdef __ASSEMBLER__
// clang-format off

#if __MNO_VZEROUPPER__ + 0
#define vzeroupper
#endif

//	Shorthand notation for widely-acknowledged sections.
.macro	.rodata
	.section .rodata,"a",@progbits
.endm
.macro	.init
	.section .init,"ax",@progbits
.endm
.macro	.real
	.section .text.real,"ax",@progbits
.endm
.macro	.head
	.section .text.head,"ax",@progbits
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

//	Mergeable numeric constant sections.
//
//	@note	linker de-dupes item/values across whole compile
//	@note	therefore item/values are reordered w.r.t. link order
//	@note	therefore no section relative addressing
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

//	Mergeable NUL-terminated UTF-8 string constant section.
//
//	@note	linker de-dupes C strings here across whole compile
//	@note	therefore item/values are reordered w.r.t. link order
//	@note	therefore no section relative addressing
.macro	.rodata.str1.1
	.section .rodata.str1.1,"aMS",@progbits,1
	.align	1
.endm

//	Locates unreferenced code invulnerable to --gc-sections.
.macro	.keep.text
	.section .keep.text,"ax",@progbits
.endm

//	Flags code as only allowed for testing purposes.
.macro	.testonly
	.section .test,"ax",@progbits
.endm

//	Makes code runnable while code morphing.
.macro	.privileged
	.section .privileged,"ax",@progbits
.endm

//	Loads address of errno into %rcx
.macro	.errno
	call	__errno_location
.endm

//	Post-Initialization Read-Only (PIRO) BSS section.
//	@param	ss is an optional string, for control image locality
.macro	.piro	ss
 .ifnb	\ss
	.section .piro.sort.bss.\ss,"aw",@nobits
 .else
	.section .piro.bss,"aw",@nobits
 .endif
.endm

//	Helpers for Cosmopolitan _init() amalgamation magic.
//	@param	name should be consistent across macros for a module
//	@see	libc/runtime/_init.S
.macro	.initro number:req name:req
	.section ".initro.\number\().\name","a",@progbits
	.align	8
.endm
.macro	.initbss number:req name:req
	.section ".piro.bss.init.2.\number\().\name","aw",@nobits
	.align	8
.endm
.macro	.init.start number:req name:req
	.section ".init.\number\().\name","ax",@progbits
"\name":
.endm
.macro	.init.end number:req name:req bnd=globl vis
	.endfn	"\name",\bnd,\vis
	.previous
.endm

//	Declares alternative implementation of function.
//	@param	implement e.g. tinymath_pow
//	@param	canonical e.g. pow
.macro	.alias	implement:req canonical:req
	.equ	\canonical,\implement
	.weak	\canonical
.endm

//	Ends function definition.
//	@cost	saves 1-3 lines of code
.macro	.endfn	name:req bnd vis
 .size	"\name",.-"\name"
 .type	"\name",@function
 .ifnb	\bnd
  .\bnd	"\name"
 .endif
 .ifnb	\vis
  .\vis	"\name"
 .endif
.endm

//	Ends variable definition.
//	@cost	saves 1-3 lines of code
.macro	.endobj	name:req bnd vis
 .size	"\name",.-"\name"
 .type	"\name",@object
 .ifnb	\bnd
  .\bnd	"\name"
 .endif
 .ifnb	\vis
  .\vis	"\name"
 .endif
.endm

//	LOOP Instruction Replacement.
.macro	.loop	label:req
	.byte	0x83
	.byte	0xe9
	.byte	0x01
	jnz	\label
.endm

//	Pushes CONSTEXPR ∈ [-128,127].
//	@note	assembler is wrong for non-literal constexprs
.macro	pushb	x:req
	.byte	0x6a
	.byte	\x
.endm

//	Sign-extends CONSTEXPR ∈ [-128,127] to REGISTER.
//	@cost	≥1 cycles, -2 bytes
.macro	pushpop	constexpr:req register:req
	pushb	\constexpr
	pop	\register
.endm

//	Moves REGISTER to REGISTER.
//	@cost	≥1 cycles, -1 REX byte
.macro	movpp	src:req dest:req
	push	\src
	pop	\dest
.endm

//	Declares optional function.
.macro	.optfn	fn:req
	.globl	"\fn"
	.weak	"\fn"
	.equ	"\fn",_missingno
	.type	"\fn",@function
.endm

//	Embeds fixed-width zero-filled string table.
//	@note	zero-padded ≠ nul-terminated
.macro	.fxstr	width head rest:vararg
 .ifnb	\head
0:	.ascii	"\head"
	.org	0b+\width
	.fxstr	\width,\rest
 .endif
.endm

//	Embeds Fixed-Width Zero-Padded String.
//	@note	.fxstr is better
.macro	.ascin str:req fieldsize:req
1347:	.ascii	"\str"
 .org	1347b+\fieldsize,0x00
.endm

//	Marks symbols as object en-masse.
//	@note	zero-padded ≠ nul-terminated
.macro	.object	symbol rest:vararg
 .ifnb	\symbol
	.type	\symbol,@object
	.object	\rest
 .endif
.endm

//	Pads function prologue unconditionally for runtime hooking.
//	@cost	≥0.3 cycles, 5 bytes
//	@see	.profilable
.macro	.hookable
	.byte	0x0f
	.byte	0x1f
	.byte	0x44
	.byte	0x00
	.byte	0x00
.endm

//	Puts initialized data in uninitialized data section.
.macro	.bsdata	name:req expr:req bnd vis
  .section ".initbss.300._init_\name","aw",@nobits
"\name":
	.quad	0
	.endobj	"\name",\bnd,\vis
  .previous
  .section ".initro.300._init_\name","a",@progbits
	.quad	\expr
  .previous
  .section ".init.300._init_\name","ax",@progbits
"_init_\name":
	movsq
	.endfn	"_init_\name"
  .previous
.endm

//	ICE Breakpoint.
//	Modern gas forgot this but objdump knows
//	@mode	long,legacy,real
	.macro	icebp
	.byte	0xF1
	.endm
	.macro	int1
	icebp
	.endm

//	Sets breakpoint for software debugger.
//	@mode	long,legacy,real
.macro	.softicebp
	.byte	0x53		# push bx
	.byte	0x87		# xchg bx,bx (bochs breakpoint)
	.byte	0xdb
	.byte	0x5b		# pop bx
	.byte	0x66		# xchg ax,ax (microsoft breakpoint)
	.byte	0x90
	int3			# gdb breakpoint
.endm

//	Assembles Intel Official 4-Byte NOP.
.macro	fatnop4
 .byte	0x0f,0x1f,0x40,0x00
.endm

//	Pulls unrelated module into linkage.
//
//	In order for this technique to work with --gc-sections, another
//	module somewhere might want to weakly reference whats yoinked.
.macro	.yoink	symbol:req
	.section .yoink
	nopl	"\symbol"(%rip)
	.previous
.endm

//	Calls Windows function.
//
//	@param	cx,dx,r8,r9,stack
//	@return	ax
//	@clob	ax,cx,dx,r8-r11
.macro	ntcall	symbol:req
	sub	$32,%rsp
	call	*\symbol(%rip)
	add	$32,%rsp
.endm

//	Custom emulator instruction for bottom stack frame.
.macro	bofram	endfunc:req
	.byte	0x0f,0x1f,0105,\endfunc-.	# nopl disp8(%rbp)
.endm

//	Begins definition of frameless function that calls no functions.
.macro	.leafprologue
#if !(defined(TINY) && !defined(__PG__))
	push	%rbp
	mov	%rsp,%rbp
#endif
.endm

//	Ends definition of frameless function that calls no functions.
.macro	.leafepilogue
#if !(defined(TINY) && !defined(__PG__))
	pop	%rbp
#endif
	ret
.endm

//	Good alignment for functions where alignment actually helps.
//	@note 16-byte
.macro	.alignfunc
#ifndef	__OPTIMIZE_SIZE__
	.p2align 4
#endif
.endm

//	Good alignment for loops where alignment actually helps.
//	@note 16-byte if <10 padding otherwise 8-byte
.macro	.alignloop
#ifndef	__OPTIMIZE_SIZE__
	.p2align 4,,10
	.p2align 4
#endif
.endm

//	Loads Effective Address
//	Supporting security blankets
.macro	plea	symbol:req reg64:req reg32:req
#if	__PIC__ + __PIE__ + __code_model_medium__ + __code_model_large__ + 0 > 1
	lea	\symbol(%rip),\reg64
#else
	mov	$\symbol,\reg32
#endif
.endm

//	Loads Effective Address to Stack
//	Supporting security blankets
.macro	pshaddr	symbol:req
#if	__PIC__ + __PIE__ + __code_model_medium__ + __code_model_large__ + 0 > 1
	push	$IMAGE_BASE_VIRTUAL+RVA(\symbol)(%rip),\reg64
#else
	push	$\symbol
#endif
.endm

//	TODO(jart): delete
//	Loads Effective Address
//	Supporting security blankets
.macro	ezlea	symbol:req reg:req
#if	__pic__ + __pie__ + __code_model_medium__ + __code_model_large__ + 0 > 1
//	lea	\symbol(%rip),%r\reg
	mov	$\symbol,%e\reg
#else
	mov	$\symbol,%e\reg
#endif
.endm

.macro	farcall	symbol:req
 .type	\symbol,@function
#if	__PIC__ + __PIE__ + __code_model_medium__ + __code_model_large__ + 0 > 1
	call	*\symbol\()@gotpcrel(%rip)
#else
	call	\symbol\()@plt
#endif
.endm

//	Documents unreachable assembly code.
.macro	.unreachable
#ifndef NDEBUG
	ud2		# crash if contract is broken
#elif defined(__FNO_OMIT_FRAME_POINTER__)
	nop		# avoid noreturn tail call backtrace ambiguity
#endif
.endm

//	Inserts profiling hook in prologue if cc wants it.
//
//	Cosmopolitan does this in a slightly different way from normal
//	GNU toolchains. We always use the -mnop-mcount behavior, since
//	the runtime is able to morph the binary at runtime. It is good
//	since we can put hooks for profiling and function tracing into
//	most builds, without any impact on performance.
//
//	@cost	≥0.3 cycles, 5 bytes
//	@see	build/compile
.macro	.profilable
#ifdef __PG__
1382:
#if defined(__MFENTRY__)
	call	__fentry__
#elif defined(__PIC__) || defined(__PIE__)
//	nopw 0x00(%rax,%rax,1)
	.byte	0x66,0x0f,0x1f,0x44,0x00,0x00
#else
//	nopl 0x00(%rax,%rax,1)
	.byte	0x0f,0x1f,0x44,0x00,0x00
#endif
#endif
.endm

//	Pushes RVA on stack of linktime mergeable string literal.
//	@see	popstr
.macro	pushstr	text
	.section .rodata.str1.1,"aSM",@progbits,1
.Lstr\@: .asciz	"\text"
	.endobj	.Lstr\@
	.previous
	push	$.Lstr\@ - IMAGE_BASE_VIRTUAL
.endm

//	Pops off stack string address.
//	@see	pushstr
.macro	popstr	dest:req
	addl	$IMAGE_BASE_VIRTUAL,(%rsp)
	pop	\dest
.endm

//	Loads address of linktime mergeable string literal into register.
.macro	loadstr	text:req reg:req regsz bias=0
 .section .rodata.str1.1,"aSM",@progbits,1
  .type	.Lstr\@,@object
.Lstr\@: .asciz	"\text"
.Lstr\@.size = .-.Lstr\@ - 1
  .size	.Lstr\@,.-.Lstr\@
 .previous
	ezlea	.Lstr\@,\reg
 .ifnb	\regsz
#ifdef	__OPTIMIZE_SIZE__
  .if	.Lstr\@.size + \bias < 128
	pushpop	.Lstr\@.size,%r\regsz
  .else
	mov	$.Lstr\@.size,%e\regsz
  .endif
#else
	mov	$.Lstr\@.size,%e\regsz
#endif
 .endif
.endm

.macro	.poison	name:req kind:req
#ifdef __SANITIZE_ADDRESS__
2323:	.quad	0
	.init.start 304,"_init_\name\()_poison_\@"
	push	%rdi
	push	%rsi
	ezlea	2323b,di
	mov	$8,%esi
	mov	$\kind,%edx
	call	__asan_poison
	pop	%rsi
	pop	%rdi
	.init.end 304,"_init_\name\()_poison_\@"
#endif
.endm

.macro	.underrun
#ifdef __SANITIZE_ADDRESS__
	.poison	__BASE_FILE__, -20  # kAsanGlobalUnderrun
#endif
.endm

.macro	.overrun
#ifdef __SANITIZE_ADDRESS__
	.poison	__BASE_FILE__, -21  # kAsanGlobalOverrun
#endif
.endm

// clang-format on
#endif /* __ASSEMBLER__ */
#endif /* COSMOPOLITAN_LIBC_MACROS_H_ */
