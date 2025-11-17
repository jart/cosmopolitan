#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
eafnosupport:
	.ftrace2
#ifdef __x86_64__
	push	$EAFNOSUPPORT
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EAFNOSUPPORT
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eafnosupport,globl,hidden
