#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
emsgsize:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EMSGSIZE(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EMSGSIZE
	ldrsw	x0,[x1,#:lo12:EMSGSIZE]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	emsgsize,globl,hidden
