#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
ealready:
	.ftrace2
#ifdef __x86_64__
	push	$EALREADY
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EALREADY
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	ealready,globl,hidden
