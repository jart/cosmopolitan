#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
enomsg:
	.ftrace2
#ifdef __x86_64__
	push	$ENOMSG
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ENOMSG
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enomsg,globl,hidden
