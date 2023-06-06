#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
eremchg:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EREMCHG(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EREMCHG
	ldrsw	x0,[x1,#:lo12:EREMCHG]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eremchg,globl,hidden
