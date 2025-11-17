#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
estale:
	.ftrace2
#ifdef __x86_64__
	push	$ESTALE
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ESTALE
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	estale,globl,hidden
