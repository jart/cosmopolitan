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

.macro	.scall	name:req amd:req arm_linux:req arm_xnu:req kw1 kw2
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
  .ifc \arm_linux,4095
    .ifc \arm_xnu,4095
//	return enosys();
	.ftrace1
\name:	.ftrace2
	b	enosys
	.endfn	\name,\kw1,\kw2
  .else
//	return IsXnu() ? syscall(x16, ...) : syscall(x8, ...);
	.ftrace1
\name:	.ftrace2
	adrp	x9,__hostos
	ldr	w9,[x9,#:lo12:__hostos]
	tbz	x9,#3,1f			// !IsXnu()
	mov	x16,#\arm_xnu			// apple ordinal
	mov	x9,#0				// clear carry flag
	adds	x9,x9,#0			// clear carry flag
	svc	#0				// issue system call
	bcs	1f
	b	_sysret
1:	neg	x0,x0
	b	_sysret
	.hidden	_sysret
	.endfn	\name,\kw1,\kw2
    .endif
  .else
    .ifc \arm_xnu,4095
//	return IsLinux() ? syscall(x8, ...) : enosys();
	.ftrace1
\name:	.ftrace2
	adrp	x9,__hostos
	ldr	w9,[x9,#:lo12:__hostos]
	tbz	x9,#0,1f			// !IsLinux()
	mov	x8,#\arm_linux			// systemd ordinal
	svc	#0				// issue system call
	mov	x1,#\arm_linux
	b	_sysret
	.hidden	_sysret
1:	b	enosys
	.endfn	\name,\kw1,\kw2
  .else
	.ftrace1
\name:	.ftrace2
	mov	x16,#\arm_xnu			// apple ordinal
	mov	x8,#\arm_linux			// systemd ordinal
	mov	x9,#0				// clear carry flag
	adds	x9,x9,#0			// clear carry flag
	svc	#0				// issue system call
	bcs	1f
	b	_sysret
1:	neg	x0,x0
	b	_sysret
	.hidden	_sysret
	.endfn	\name,\kw1,\kw2
    .endif
  .endif
#else
#error "architecture unsupported"
#endif
	.previous
.endm

/* clang-format on */
#endif /* __ASSEMBLER__ */
#endif /* COSMOPOLITAN_LIBC_SYSV_MACROS_H_ */
