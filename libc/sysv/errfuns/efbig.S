#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
efbig:
	.ftrace2
#ifdef __x86_64__
	push	$EFBIG
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EFBIG
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	efbig,globl,hidden
