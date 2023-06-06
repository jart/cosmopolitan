#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
enobufs:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ENOBUFS(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENOBUFS
	ldrsw	x0,[x1,#:lo12:ENOBUFS]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enobufs,globl,hidden
