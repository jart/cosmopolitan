#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
eacces:
	.ftrace2
#ifdef __x86_64__
	push	$EACCES
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EACCES
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eacces,globl,hidden
