#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
esrmnt:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ESRMNT(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ESRMNT
	ldrsw	x0,[x1,#:lo12:ESRMNT]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	esrmnt,globl,hidden
