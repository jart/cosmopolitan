#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
edom:
	.ftrace2
#ifdef __x86_64__
	push	$EDOM
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EDOM
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	edom,globl,hidden
