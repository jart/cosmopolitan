#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
econnrefused:
	.ftrace2
#ifdef __x86_64__
	push	$ECONNREFUSED
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ECONNREFUSED
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	econnrefused,globl,hidden
