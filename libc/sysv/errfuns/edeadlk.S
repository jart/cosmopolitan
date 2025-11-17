#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
edeadlk:
	.ftrace2
#ifdef __x86_64__
	push	$EDEADLK
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EDEADLK
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	edeadlk,globl,hidden
