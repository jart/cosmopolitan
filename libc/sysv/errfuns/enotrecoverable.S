#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
enotrecoverable:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ENOTRECOVERABLE(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENOTRECOVERABLE
	ldrsw	x0,[x1,#:lo12:ENOTRECOVERABLE]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enotrecoverable,globl,hidden
