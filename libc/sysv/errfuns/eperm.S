#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
eperm:
	.ftrace2
#ifdef __x86_64__
	push	$EPERM
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EPERM
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eperm,globl,hidden
