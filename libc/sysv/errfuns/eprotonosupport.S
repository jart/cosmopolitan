#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
eprotonosupport:
	.ftrace2
#ifdef __x86_64__
	push	$EPROTONOSUPPORT
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EPROTONOSUPPORT
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eprotonosupport,globl,hidden
