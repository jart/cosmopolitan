#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
emediumtype:
	.ftrace2
#ifdef __x86_64__
	push	$EMEDIUMTYPE
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EMEDIUMTYPE
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	emediumtype,globl,hidden
