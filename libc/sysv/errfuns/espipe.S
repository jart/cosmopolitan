#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
espipe:
	.ftrace2
#ifdef __x86_64__
	push	$ESPIPE
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ESPIPE
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	espipe,globl,hidden
