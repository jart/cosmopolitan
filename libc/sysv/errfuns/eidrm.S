#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
eidrm:
	.ftrace2
#ifdef __x86_64__
	push	$EIDRM
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EIDRM
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eidrm,globl,hidden
