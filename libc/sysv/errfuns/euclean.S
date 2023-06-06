#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
euclean:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EUCLEAN(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EUCLEAN
	ldrsw	x0,[x1,#:lo12:EUCLEAN]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	euclean,globl,hidden
