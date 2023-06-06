#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
eunatch:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EUNATCH(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EUNATCH
	ldrsw	x0,[x1,#:lo12:EUNATCH]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eunatch,globl,hidden
