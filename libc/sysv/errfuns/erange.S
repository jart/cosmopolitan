#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
erange:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ERANGE(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ERANGE
	ldrsw	x0,[x1,#:lo12:ERANGE]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	erange,globl,hidden
