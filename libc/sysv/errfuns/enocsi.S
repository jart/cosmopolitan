#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
enocsi:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ENOCSI(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENOCSI
	ldrsw	x0,[x1,#:lo12:ENOCSI]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enocsi,globl,hidden
