#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
eexist:
	.ftrace2
#ifdef __x86_64__
	push	$EEXIST
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EEXIST
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eexist,globl,hidden
