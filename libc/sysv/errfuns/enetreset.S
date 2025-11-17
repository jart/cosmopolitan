#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
enetreset:
	.ftrace2
#ifdef __x86_64__
	push	$ENETRESET
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ENETRESET
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enetreset,globl,hidden
