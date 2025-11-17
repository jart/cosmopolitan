#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
eintr:
	.ftrace2
#ifdef __x86_64__
	push	$EINTR
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EINTR
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eintr,globl,hidden
