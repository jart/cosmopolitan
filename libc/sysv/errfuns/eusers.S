#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
eusers:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EUSERS(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EUSERS
	ldrsw	x0,[x1,#:lo12:EUSERS]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eusers,globl,hidden
