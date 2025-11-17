#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
etoomanyrefs:
	.ftrace2
#ifdef __x86_64__
	push	$ETOOMANYREFS
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ETOOMANYREFS
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	etoomanyrefs,globl,hidden
