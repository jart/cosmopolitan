#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
eprototype:
	.ftrace2
#ifdef __x86_64__
	push	$EPROTOTYPE
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EPROTOTYPE
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eprototype,globl,hidden
