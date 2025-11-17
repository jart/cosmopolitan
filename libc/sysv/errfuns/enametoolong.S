#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
enametoolong:
	.ftrace2
#ifdef __x86_64__
	push	$ENAMETOOLONG
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ENAMETOOLONG
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enametoolong,globl,hidden
