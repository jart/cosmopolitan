#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
e2big:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	E2BIG(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,E2BIG
	ldrsw	x0,[x1,#:lo12:E2BIG]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	e2big,globl,hidden
