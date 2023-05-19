#include "libc/macros.internal.h"
.text.unlikely

eidrm:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EIDRM(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EIDRM
	ldrsw	x0,[x1,#:lo12:EIDRM]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eidrm,globl,hidden
