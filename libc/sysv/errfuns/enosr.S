#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
enosr:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ENOSR(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENOSR
	ldrsw	x0,[x1,#:lo12:ENOSR]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enosr,globl,hidden
