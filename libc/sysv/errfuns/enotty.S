#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
enotty:
	.ftrace2
#ifdef __x86_64__
	push	$ENOTTY
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ENOTTY
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enotty,globl,hidden
