#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
emlink:
	.ftrace2
#ifdef __x86_64__
	push	$EMLINK
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EMLINK
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	emlink,globl,hidden
