#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
enobufs:
	.ftrace2
#ifdef __x86_64__
	push	$ENOBUFS
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ENOBUFS
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enobufs,globl,hidden
