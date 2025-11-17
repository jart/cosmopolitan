#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
erestart:
	.ftrace2
#ifdef __x86_64__
	push	$ERESTART
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ERESTART
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	erestart,globl,hidden
