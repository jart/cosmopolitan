#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
edotdot:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EDOTDOT(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EDOTDOT
	ldrsw	x0,[x1,#:lo12:EDOTDOT]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	edotdot,globl,hidden
