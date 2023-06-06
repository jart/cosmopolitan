#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
enotsock:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ENOTSOCK(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENOTSOCK
	ldrsw	x0,[x1,#:lo12:ENOTSOCK]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enotsock,globl,hidden
