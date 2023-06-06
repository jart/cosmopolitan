#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
enodev:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ENODEV(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENODEV
	ldrsw	x0,[x1,#:lo12:ENODEV]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enodev,globl,hidden
