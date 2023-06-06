#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
econnrefused:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ECONNREFUSED(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ECONNREFUSED
	ldrsw	x0,[x1,#:lo12:ECONNREFUSED]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	econnrefused,globl,hidden
