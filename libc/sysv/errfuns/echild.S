#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
echild:
	.ftrace2
#ifdef __x86_64__
	push	$ECHILD
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ECHILD
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	echild,globl,hidden
