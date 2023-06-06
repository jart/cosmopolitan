#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
eremoteio:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EREMOTEIO(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EREMOTEIO
	ldrsw	x0,[x1,#:lo12:EREMOTEIO]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eremoteio,globl,hidden
