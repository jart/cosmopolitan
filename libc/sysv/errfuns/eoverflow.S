#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
eoverflow:
	.ftrace2
#ifdef __x86_64__
	push	$EOVERFLOW
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EOVERFLOW
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eoverflow,globl,hidden
