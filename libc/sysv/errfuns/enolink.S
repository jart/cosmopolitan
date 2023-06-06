#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
enolink:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ENOLINK(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENOLINK
	ldrsw	x0,[x1,#:lo12:ENOLINK]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enolink,globl,hidden
