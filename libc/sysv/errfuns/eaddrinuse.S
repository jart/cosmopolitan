#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
eaddrinuse:
	.ftrace2
#ifdef __x86_64__
	push	$EADDRINUSE
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EADDRINUSE
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eaddrinuse,globl,hidden
