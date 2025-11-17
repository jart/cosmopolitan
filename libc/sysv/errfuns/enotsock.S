#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
enotsock:
	.ftrace2
#ifdef __x86_64__
	push	$ENOTSOCK
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ENOTSOCK
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enotsock,globl,hidden
