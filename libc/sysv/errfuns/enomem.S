#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
enomem:
	.ftrace2
#ifdef __x86_64__
	push	$ENOMEM
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ENOMEM
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enomem,globl,hidden
