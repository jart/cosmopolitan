#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
enavail:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ENAVAIL(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENAVAIL
	ldrsw	x0,[x1,#:lo12:ENAVAIL]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enavail,globl,hidden
