#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
ebadslt:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EBADSLT(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EBADSLT
	ldrsw	x0,[x1,#:lo12:EBADSLT]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	ebadslt,globl,hidden
