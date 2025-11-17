#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
eusers:
	.ftrace2
#ifdef __x86_64__
	push	$EUSERS
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EUSERS
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eusers,globl,hidden
