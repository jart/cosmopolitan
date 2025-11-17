#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
eopnotsupp:
	.ftrace2
#ifdef __x86_64__
	push	$EOPNOTSUPP
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EOPNOTSUPP
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eopnotsupp,globl,hidden
