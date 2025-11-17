#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
einval:
	.ftrace2
#ifdef __x86_64__
	push	$EINVAL
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EINVAL
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	einval,globl,hidden
