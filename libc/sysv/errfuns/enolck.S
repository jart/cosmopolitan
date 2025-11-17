#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
enolck:
	.ftrace2
#ifdef __x86_64__
	push	$ENOLCK
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ENOLCK
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enolck,globl,hidden
