#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
enoent:
	.ftrace2
#ifdef __x86_64__
	push	$ENOENT
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ENOENT
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enoent,globl,hidden
