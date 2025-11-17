#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
enetdown:
	.ftrace2
#ifdef __x86_64__
	push	$ENETDOWN
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ENETDOWN
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enetdown,globl,hidden
