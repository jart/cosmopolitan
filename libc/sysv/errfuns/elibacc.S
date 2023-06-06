#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
elibacc:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ELIBACC(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ELIBACC
	ldrsw	x0,[x1,#:lo12:ELIBACC]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	elibacc,globl,hidden
