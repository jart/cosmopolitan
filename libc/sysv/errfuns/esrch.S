#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
esrch:
	.ftrace2
#ifdef __x86_64__
	push	$ESRCH
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ESRCH
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	esrch,globl,hidden
