#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
efault:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EFAULT(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EFAULT
	ldrsw	x0,[x1,#:lo12:EFAULT]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	efault,globl,hidden
