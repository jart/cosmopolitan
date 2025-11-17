#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
eremote:
	.ftrace2
#ifdef __x86_64__
	push	$EREMOTE
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EREMOTE
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eremote,globl,hidden
