#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
eloop:
	.ftrace2
#ifdef __x86_64__
	push	$ELOOP
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ELOOP
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eloop,globl,hidden
