#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
ehostunreach:
	.ftrace2
#ifdef __x86_64__
	push	$EHOSTUNREACH
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EHOSTUNREACH
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	ehostunreach,globl,hidden
