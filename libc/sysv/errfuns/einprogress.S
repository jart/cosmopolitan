#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
einprogress:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EINPROGRESS(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EINPROGRESS
	ldrsw	x0,[x1,#:lo12:EINPROGRESS]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	einprogress,globl,hidden
