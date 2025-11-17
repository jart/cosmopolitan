#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
enostr:
	.ftrace2
#ifdef __x86_64__
	push	$ENOSTR
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ENOSTR
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enostr,globl,hidden
