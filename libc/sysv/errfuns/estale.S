#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
estale:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ESTALE(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ESTALE
	ldrsw	x0,[x1,#:lo12:ESTALE]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	estale,globl,hidden
