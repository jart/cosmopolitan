#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
edom:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EDOM(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EDOM
	ldrsw	x0,[x1,#:lo12:EDOM]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	edom,globl,hidden
