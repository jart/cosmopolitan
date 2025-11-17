#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
eshutdown:
	.ftrace2
#ifdef __x86_64__
	push	$ESHUTDOWN
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ESHUTDOWN
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eshutdown,globl,hidden
