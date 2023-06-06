#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
ecanceled:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ECANCELED(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ECANCELED
	ldrsw	x0,[x1,#:lo12:ECANCELED]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	ecanceled,globl,hidden
