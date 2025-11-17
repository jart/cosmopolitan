#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
econnreset:
	.ftrace2
#ifdef __x86_64__
	push	$ECONNRESET
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ECONNRESET
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	econnreset,globl,hidden
