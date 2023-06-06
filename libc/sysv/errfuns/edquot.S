#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
edquot:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EDQUOT(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EDQUOT
	ldrsw	x0,[x1,#:lo12:EDQUOT]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	edquot,globl,hidden
