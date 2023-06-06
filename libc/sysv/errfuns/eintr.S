#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
eintr:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EINTR(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EINTR
	ldrsw	x0,[x1,#:lo12:EINTR]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eintr,globl,hidden
