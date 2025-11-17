#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
erange:
	.ftrace2
#ifdef __x86_64__
	push	$ERANGE
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ERANGE
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	erange,globl,hidden
