#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
eopnotsupp:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EOPNOTSUPP(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EOPNOTSUPP
	ldrsw	x0,[x1,#:lo12:EOPNOTSUPP]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eopnotsupp,globl,hidden
