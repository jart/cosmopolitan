#ifndef COSMOPOLITAN_LIBC_SYSV_MACROS_H_
#define COSMOPOLITAN_LIBC_SYSV_MACROS_H_
#include "libc/macros.internal.h"
#ifdef __ASSEMBLER__
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

.macro	.scall	name:req amd:req arm_linux:req arm_xnu:req arm_freebsd:req kw1 kw2
	.section .text.syscall,"ax",@progbits
#ifdef __x86_64__
  .ifnb	\kw2
	.ftrace1
\name:	.ftrace2
	movabs	$\amd,%rax
	jmp	*__systemfive(%rip)
  .else
	.ftrace1
\name:	.ftrace2
	push	%rbp
	mov	%rsp,%rbp
	movabs	$\amd,%rax
	call	*__systemfive(%rip)
	pop	%rbp
	ret
  .endif
	.endfn	\name,\kw1,\kw2
#elif defined(__aarch64__)
	.ftrace1
\name:	.ftrace2
	mov	x8,#\arm_linux
	mov	x9,#\arm_freebsd
	mov	x16,#\arm_xnu
	b	systemfive
	.endfn	\name,\kw1,\kw2
#else
#error "architecture unsupported"
#endif
	.previous
.endm

/* clang-format on */
#endif /* __ASSEMBLER__ */
#endif /* COSMOPOLITAN_LIBC_SYSV_MACROS_H_ */
