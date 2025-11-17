#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
enotconn:
	.ftrace2
#ifdef __x86_64__
	push	$ENOTCONN
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ENOTCONN
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enotconn,globl,hidden
