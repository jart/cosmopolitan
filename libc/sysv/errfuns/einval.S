#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
einval:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EINVAL(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EINVAL
	ldrsw	x0,[x1,#:lo12:EINVAL]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	einval,globl,hidden
