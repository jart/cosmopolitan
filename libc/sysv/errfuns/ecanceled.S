#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
ecanceled:
	.ftrace2
#ifdef __x86_64__
	push	$ECANCELED
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ECANCELED
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	ecanceled,globl,hidden
