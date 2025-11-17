#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
eisdir:
	.ftrace2
#ifdef __x86_64__
	push	$EISDIR
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EISDIR
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eisdir,globl,hidden
