#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
einprogress:
	.ftrace2
#ifdef __x86_64__
	push	$EINPROGRESS
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EINPROGRESS
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	einprogress,globl,hidden
