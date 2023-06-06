#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
exfull:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EXFULL(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EXFULL
	ldrsw	x0,[x1,#:lo12:EXFULL]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	exfull,globl,hidden
