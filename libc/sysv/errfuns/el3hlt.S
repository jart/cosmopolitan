#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
el3hlt:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EL3HLT(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EL3HLT
	ldrsw	x0,[x1,#:lo12:EL3HLT]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	el3hlt,globl,hidden
