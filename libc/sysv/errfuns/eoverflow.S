#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
eoverflow:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EOVERFLOW(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EOVERFLOW
	ldrsw	x0,[x1,#:lo12:EOVERFLOW]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eoverflow,globl,hidden
