#ifndef APE_MACROS_H_
#define APE_MACROS_H_
#include "libc/macros.h"
#ifdef __ASSEMBLER__
/* clang-format off */

/**
 * @fileoverview Macros relevant to αcτµαlly pδrταblε εxεcµταblε.
 */

/	Calls near (i.e. pc+pcrel<64kB) FUNCTION.
/	@mode	long,legacy,real
/	@cost	9 bytes overhead
.macro	rlcall	function:req
	.byte	0x50                   	# push %[er]ax
	.byte	0xb8,0,0		# mov $?,%[e]ax
	jmp	911f
	.byte	0x58                   	# pop %[er]ax
	.byte	0xe8			# call Jvds
	.long	\function\()-.-4
	jmp	912f
911:	.byte	0x58                   	# pop %[er]ax
	.byte	0xe8			# call Jvds
	.short	\function\()-.-2
912:
.endm

/	Loads far (i.e. <1mb) abs constexpr ADDRESS into ES:DI+EDX+RDX.
/	@mode	long,legacy,real
.macro	movesdi	address:req
	.byte	0xbf			# mov $0x????xxxx,%[e]di
	.short	\address>>4
	.byte	0x8e,0xc7		# mov %di,%es
	.byte	0xbf			# mov $0x????xxxx,%[e]di
	.short	\address&0xf
	jmp	297f
	.byte	0xbf			# mov $0x????xxxx,%edi
	.long	\address
297:	
.endm

/	Loads 16-bit CONSTEXPR into Qw-register w/ optional zero-extend.
/	@mode	long,legacy,real
.macro	bbmov	constexpr:req abcd abcd.hi:req abcd.lo:req
 .ifnb	\abcd
  .if	(\constexpr)<128 && (\constexpr)>=0
	pushpop	\constexpr,\abcd
   .exitm
  .endif
 .endif
	movb	$(\constexpr)>>8&0xff,\abcd.hi
	movb	$(\constexpr)&0xff,\abcd.lo
.endm

/	Compares 16-bit CONSTEXPR with Qw-register.
/	@mode	long,legacy,real
.macro	bbcmp	constexpr:req abcd.hi:req abcd.lo:req
	cmpb	$(\constexpr)>>8&0xff,\abcd.hi
	jnz	387f
	cmpb	$(\constexpr)&0xff,\abcd.lo
387:
.endm

/	Adds 16-bit CONSTEXPR to Qw-register.
/	@mode	long,legacy,real
.macro	bbadd	constexpr:req abcd.hi:req abcd.lo:req
	addb	$(\constexpr)&0xff,\abcd.lo
 .if	(\constexpr) != 0
	adcb	$(\constexpr)>>8&0xff,\abcd.hi
 .endif
.endm

/	Subtracts 16-bit CONSTEXPR from Qw-register.
/	@mode	long,legacy,real
.macro	bbsub	constexpr:req abcd.hi:req abcd.lo:req
	subb	$(\constexpr)&0xff,\abcd.lo
 .if	(\constexpr) != 0
	sbbb	$(\constexpr)>>8&0xff,\abcd.hi
 .endif
.endm

/	Ands Qw-register with 16-bit CONSTEXPR.
/	@mode	long,legacy,real
.macro	bband	constexpr:req abcd.hi:req abcd.lo:req
 .if	((\constexpr)&0xff) != 0xff || ((\constexpr)>>8&0xff) == 0xff
	andb	$(\constexpr)&0xff,\abcd.lo
 .endif
 .if	((\constexpr)>>8&0xff) != 0xff
	andb	$(\constexpr)>>8&0xff,\abcd.hi
 .endif
.endm

/	Ors Qw-register with 16-bit CONSTEXPR.
/	@mode	long,legacy,real
.macro	bbor	constexpr:req abcd.hi:req abcd.lo:req
 .if	((\constexpr)&0xff) != 0 || ((\constexpr)>>8&0xff) != 0
	orb	$(\constexpr)&0xff,\abcd.lo
 .endif
 .if	((\constexpr)>>8&0xff) != 0
	orb	$(\constexpr)>>8&0xff,\abcd.hi
 .endif
.endm

/	Performs ACTION only if in real mode.
/	@mode	long,legacy,real
.macro	rlo	clobber:req action:vararg
990:	mov	$0,\clobber
	.if	.-990b!=3
	.error	"bad clobber or assembler mode"
	.endif
991:	\action
	.rept	2-(.-991b)
	nop
	.endr
	.if	.-991b!=2
	.error	"ACTION must be 1-2 bytes"
	.endif
.endm

/	Initializes real mode stack.
/	The most holiest of holy code.
/	@mode	real
/	@see	www.pcjs.org/pubs/pc/reference/intel/8086/
.macro	rlstack	seg:req addr:req
	cli
	mov	\seg,%ss
	mov	\addr,%sp
	sti
.endm

/	Symbolic Linker-Defined Binary Content.
.macro	.stub	name:req kind:req default type=@object
 .ifnb	\default
  .equ	\name,\default
 .endif
 .\kind	\name
 .type	\name,\type
 .weak	\name
.hidden	\name
.endm

/	Symbolic Linker-Defined Binary-Encoded-Bourne Content.
/	@param	units is the number of encoded 32-bit values to insert,
/		e.g. \000 can be encoded as 0x3030305c.
.macro	.shstub	name:req units:req
 ss	\name,0
 .if	\units>1
  ss	\name,1
   .if	\units>2
    ss	\name,2
    ss	\name,3
    .if	\units>4
     ss	\name,4
     ss	\name,5
     ss	\name,6
     ss	\name,7
   .endif
  .endif
 .endif
.endm
.macro	ss	name n
 .stub	\name\()_bcs\n,long
.endm

/* clang-format on */
#elif defined(__LINKER__)

#define BCX_NIBBLE(X) ((((X)&0xf) > 0x9) ? ((X)&0xf) + 0x37 : ((X)&0xf) + 0x30)
#define BCX_OCTET(X) ((BCX_NIBBLE((X) >> 4) << 8) | (BCX_NIBBLE((X) >> 0) << 0))
#define BCX_INT16(X) ((BCX_OCTET((X) >> 8) << 16) | (BCX_OCTET((X) >> 0) << 0))
#define BCXSTUB(SYM, X)                      \
  HIDDEN(SYM##_bcx0 = BCX_INT16((X) >> 48)); \
  HIDDEN(SYM##_bcx1 = BCX_INT16((X) >> 32)); \
  HIDDEN(SYM##_bcx2 = BCX_INT16((X) >> 16)); \
  HIDDEN(SYM##_bcx3 = BCX_INT16((X) >> 0))

/**
 * Binary coded backslash octet support.
 *
 * <p>This allows linker scripts to generate printf commands.
 */
#define BCO_OCTET(X) (((X)&0x7) + 0x30)
#define BCOB_UNIT(X)                                           \
  ((BCO_OCTET((X) >> 0) << 24) | (BCO_OCTET((X) >> 3) << 16) | \
   (BCO_OCTET(((X)&0xff) >> 6) << 8) | 0x5c)

#define PFBYTE(SYM, X, I) HIDDEN(SYM##_bcs##I = BCOB_UNIT((X) >> ((I)*8)))
#define PFSTUB2(SYM, X) \
  HIDDEN(SYM = (X));    \
  PFBYTE(SYM, X, 0);    \
  PFBYTE(SYM, X, 1)
#define PFSTUB4(SYM, X) \
  PFSTUB2(SYM, X);      \
  PFBYTE(SYM, X, 2);    \
  PFBYTE(SYM, X, 3)
#define PFSTUB8(SYM, X) \
  PFSTUB4(SYM, X);      \
  PFBYTE(SYM, X, 4);    \
  PFBYTE(SYM, X, 5);    \
  PFBYTE(SYM, X, 6);    \
  PFBYTE(SYM, X, 7)

/**
 * Binary coded decimal support.
 *
 * <p>This allows linker scripts to generate dd commands. Leading spaces
 * need to be inserted so Mac doesn't consider them octal; therefore,
 * parameters must be quoted; and eight digits should be good enough.
 */
#define SHSTUB2(SYM, X)           \
  HIDDEN(SYM##_bcs0 = BCD10K(X)); \
  HIDDEN(SYM##_bcs1 = BCD(X))
#define BCD(X)                                                                 \
  ((X) == 0                                                                    \
       ? 0x20202030                                                            \
       : (X) < 10 ? 0x30202020 + (((X) % 10) << 24)                            \
                  : (X) < 100 ? 0x30302020 + (((X) % 10) << 24) +              \
                                    (((X) / 10 % 10) << 16)                    \
                              : (X) < 1000 ? 0x30303020 + (((X) % 10) << 24) + \
                                                 (((X) / 10 % 10) << 16) +     \
                                                 (((X) / 100 % 10) << 8)       \
                                           : 0x30303030 + (((X) % 10) << 24) + \
                                                 (((X) / 10 % 10) << 16) +     \
                                                 (((X) / 100 % 10) << 8) +     \
                                                 (((X) / 1000 % 10) << 0))
#define BCD10K(X)                                                          \
  ((X) < 10000                                                             \
       ? 0x20202020                                                        \
       : (X) < 100000                                                      \
             ? 0x30202020 + (((X) / 10000 % 10) << 24)                     \
             : (X) < 1000000                                               \
                   ? 0x30302020 + (((X) / 10000 % 10) << 24) +             \
                         (((X) / 100000 % 10) << 16)                       \
                   : (X) < 10000000                                        \
                         ? 0x30303020 + (((X) / 10000 % 10) << 24) +       \
                               (((X) / 100000 % 10) << 16) +               \
                               (((X) / 1000000 % 10) << 8)                 \
                         : (X) < 100000000                                 \
                               ? 0x30303030 + (((X) / 10000 % 10) << 24) + \
                                     (((X) / 100000 % 10) << 16) +         \
                                     (((X) / 1000000 % 10) << 8) +         \
                                     (((X) / 10000000 % 10) << 0)          \
                               : 0xffffffffffffffff)

#endif /* __ASSEMBLER__ */
#endif /* APE_MACROS_H_ */
