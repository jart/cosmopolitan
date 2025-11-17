#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
edestaddrreq:
	.ftrace2
#ifdef __x86_64__
	push	$EDESTADDRREQ
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EDESTADDRREQ
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	edestaddrreq,globl,hidden
