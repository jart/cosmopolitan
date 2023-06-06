#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
el2hlt:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EL2HLT(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EL2HLT
	ldrsw	x0,[x1,#:lo12:EL2HLT]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	el2hlt,globl,hidden
