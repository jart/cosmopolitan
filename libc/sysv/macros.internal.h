#ifndef COSMOPOLITAN_LIBC_SYSV_MACROS_H_
#define COSMOPOLITAN_LIBC_SYSV_MACROS_H_
#ifdef __ASSEMBLER__
#include "libc/macros.internal.inc"
/* clang-format off */

/**
 * @fileoverview System Five jump slot generator.
 *
 * We need to pass a magic number to the SYSCALL instruction in %rax.
 * That's only possible to do in C using asm() macro wrappers, like in
 * Chromium's "System Call Support" library. This technique is simpler
 * to implement, gains us object-level ABI compatibility, hookability,
 * and the ability to support significantly more functions, without the
 * risk of slowing down builds too much with complicated headers.
 */

.macro	.scall	name:req num:req kw1 kw2
  .ifnb	\kw2
	.align	16
\name:	movabs	$\num,%rax
	jmp	*__systemfive(%rip)
  .else
\name:	push	%rbp
	mov	%rsp,%rbp
	movabs	$\num,%rax
	.hookable
	call	*__systemfive(%rip)
	pop	%rbp
	ret
  .endif
	.endfn	\name,\kw1,\kw2
	.previous
.endm

/* clang-format on */
#endif /* __ASSEMBLER__ */
#endif /* COSMOPOLITAN_LIBC_SYSV_MACROS_H_ */
