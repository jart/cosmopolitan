#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
enoexec:
	.ftrace2
#ifdef __x86_64__
	push	$ENOEXEC
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ENOEXEC
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enoexec,globl,hidden
