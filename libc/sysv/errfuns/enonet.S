#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
enonet:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ENONET(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENONET
	ldrsw	x0,[x1,#:lo12:ENONET]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enonet,globl,hidden
