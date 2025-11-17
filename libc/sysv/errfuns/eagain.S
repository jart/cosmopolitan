#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
eagain:
	.ftrace2
#ifdef __x86_64__
	push	$EAGAIN
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EAGAIN
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eagain,globl,hidden
