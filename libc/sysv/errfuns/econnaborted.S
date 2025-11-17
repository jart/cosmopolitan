#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
econnaborted:
	.ftrace2
#ifdef __x86_64__
	push	$ECONNABORTED
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ECONNABORTED
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	econnaborted,globl,hidden
