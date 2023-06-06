#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
epipe:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EPIPE(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EPIPE
	ldrsw	x0,[x1,#:lo12:EPIPE]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	epipe,globl,hidden
