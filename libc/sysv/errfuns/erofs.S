#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
erofs:
	.ftrace2
#ifdef __x86_64__
	push	$EROFS
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EROFS
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	erofs,globl,hidden
