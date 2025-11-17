#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
enotempty:
	.ftrace2
#ifdef __x86_64__
	push	$ENOTEMPTY
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ENOTEMPTY
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enotempty,globl,hidden
