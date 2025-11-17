#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
ebadfd:
	.ftrace2
#ifdef __x86_64__
	push	$EBADFD
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EBADFD
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	ebadfd,globl,hidden
