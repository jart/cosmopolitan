#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
enolink:
	.ftrace2
#ifdef __x86_64__
	push	$ENOLINK
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ENOLINK
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enolink,globl,hidden
