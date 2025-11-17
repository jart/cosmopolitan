#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
eownerdead:
	.ftrace2
#ifdef __x86_64__
	push	$EOWNERDEAD
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EOWNERDEAD
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eownerdead,globl,hidden
