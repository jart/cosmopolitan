#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
enodev:
	.ftrace2
#ifdef __x86_64__
	push	$ENODEV
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ENODEV
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enodev,globl,hidden
