#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
enetdown:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ENETDOWN(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENETDOWN
	ldrsw	x0,[x1,#:lo12:ENETDOWN]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enetdown,globl,hidden
