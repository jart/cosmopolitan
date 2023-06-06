#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
eisnam:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EISNAM(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EISNAM
	ldrsw	x0,[x1,#:lo12:EISNAM]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eisnam,globl,hidden
