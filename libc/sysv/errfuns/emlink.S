#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
emlink:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EMLINK(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EMLINK
	ldrsw	x0,[x1,#:lo12:EMLINK]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	emlink,globl,hidden
