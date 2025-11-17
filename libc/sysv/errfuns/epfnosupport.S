#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
epfnosupport:
	.ftrace2
#ifdef __x86_64__
	push	$EPFNOSUPPORT
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EPFNOSUPPORT
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	epfnosupport,globl,hidden
