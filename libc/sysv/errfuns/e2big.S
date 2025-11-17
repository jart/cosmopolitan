#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
e2big:
	.ftrace2
#ifdef __x86_64__
	push	$E2BIG
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#E2BIG
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	e2big,globl,hidden
