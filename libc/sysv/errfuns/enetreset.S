#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
enetreset:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ENETRESET(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENETRESET
	ldrsw	x0,[x1,#:lo12:ENETRESET]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enetreset,globl,hidden
