#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
enoprotoopt:
	.ftrace2
#ifdef __x86_64__
	push	$ENOPROTOOPT
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ENOPROTOOPT
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enoprotoopt,globl,hidden
