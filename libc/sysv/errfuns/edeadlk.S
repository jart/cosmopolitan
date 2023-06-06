#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
edeadlk:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EDEADLK(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EDEADLK
	ldrsw	x0,[x1,#:lo12:EDEADLK]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	edeadlk,globl,hidden
