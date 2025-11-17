#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
eproto:
	.ftrace2
#ifdef __x86_64__
	push	$EPROTO
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EPROTO
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eproto,globl,hidden
