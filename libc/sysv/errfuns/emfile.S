#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
emfile:
	.ftrace2
#ifdef __x86_64__
	push	$EMFILE
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EMFILE
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	emfile,globl,hidden
