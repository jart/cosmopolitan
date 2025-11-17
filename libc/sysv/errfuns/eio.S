#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
eio:
	.ftrace2
#ifdef __x86_64__
	push	$EIO
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EIO
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eio,globl,hidden
