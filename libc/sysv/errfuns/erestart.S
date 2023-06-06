#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
erestart:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ERESTART(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ERESTART
	ldrsw	x0,[x1,#:lo12:ERESTART]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	erestart,globl,hidden
