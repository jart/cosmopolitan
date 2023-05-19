#include "libc/macros.internal.h"
.text.unlikely

eoverflow:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EOVERFLOW(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EOVERFLOW
	ldrsw	x0,[x1,#:lo12:EOVERFLOW]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eoverflow,globl,hidden
