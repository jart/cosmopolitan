#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
efault:
	.ftrace2
#ifdef __x86_64__
	push	$EFAULT
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EFAULT
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	efault,globl,hidden
