#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
enotty:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ENOTTY(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENOTTY
	ldrsw	x0,[x1,#:lo12:ENOTTY]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enotty,globl,hidden
