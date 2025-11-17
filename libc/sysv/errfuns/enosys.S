#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
enosys:
	.ftrace2
#ifdef __x86_64__
	push	$ENOSYS
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ENOSYS
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enosys,globl,hidden
