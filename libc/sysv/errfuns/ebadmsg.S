#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
ebadmsg:
	.ftrace2
#ifdef __x86_64__
	push	$EBADMSG
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EBADMSG
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	ebadmsg,globl,hidden
