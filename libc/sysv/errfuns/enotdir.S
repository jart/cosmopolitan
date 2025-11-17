#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
enotdir:
	.ftrace2
#ifdef __x86_64__
	push	$ENOTDIR
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ENOTDIR
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enotdir,globl,hidden
