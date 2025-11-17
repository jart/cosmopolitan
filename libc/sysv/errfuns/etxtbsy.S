#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
etxtbsy:
	.ftrace2
#ifdef __x86_64__
	push	$ETXTBSY
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ETXTBSY
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	etxtbsy,globl,hidden
