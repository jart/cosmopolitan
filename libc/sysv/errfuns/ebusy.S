#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
ebusy:
	.ftrace2
#ifdef __x86_64__
	push	$EBUSY
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EBUSY
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	ebusy,globl,hidden
