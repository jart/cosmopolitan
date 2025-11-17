#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
enomedium:
	.ftrace2
#ifdef __x86_64__
	push	$ENOMEDIUM
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ENOMEDIUM
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enomedium,globl,hidden
