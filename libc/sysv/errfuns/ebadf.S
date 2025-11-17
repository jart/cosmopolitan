#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
ebadf:
	.ftrace2
#ifdef __x86_64__
	push	$EBADF
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EBADF
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	ebadf,globl,hidden
