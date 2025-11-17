#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
enodata:
	.ftrace2
#ifdef __x86_64__
	push	$ENODATA
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ENODATA
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enodata,globl,hidden
