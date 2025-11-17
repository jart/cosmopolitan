#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
eaddrnotavail:
	.ftrace2
#ifdef __x86_64__
	push	$EADDRNOTAVAIL
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EADDRNOTAVAIL
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eaddrnotavail,globl,hidden
