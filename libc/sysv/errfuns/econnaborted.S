#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
econnaborted:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ECONNABORTED(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ECONNABORTED
	ldrsw	x0,[x1,#:lo12:ECONNABORTED]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	econnaborted,globl,hidden
