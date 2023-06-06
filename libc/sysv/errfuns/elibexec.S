#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
elibexec:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ELIBEXEC(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ELIBEXEC
	ldrsw	x0,[x1,#:lo12:ELIBEXEC]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	elibexec,globl,hidden
