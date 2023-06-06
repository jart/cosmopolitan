#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
echrng:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ECHRNG(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ECHRNG
	ldrsw	x0,[x1,#:lo12:ECHRNG]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	echrng,globl,hidden
