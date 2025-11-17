#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
enosr:
	.ftrace2
#ifdef __x86_64__
	push	$ENOSR
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ENOSR
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enosr,globl,hidden
