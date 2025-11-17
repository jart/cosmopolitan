#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
erfkill:
	.ftrace2
#ifdef __x86_64__
	push	$ERFKILL
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ERFKILL
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	erfkill,globl,hidden
