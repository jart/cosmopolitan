#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
edestaddrreq:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EDESTADDRREQ(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EDESTADDRREQ
	ldrsw	x0,[x1,#:lo12:EDESTADDRREQ]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	edestaddrreq,globl,hidden
