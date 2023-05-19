#include "libc/macros.internal.h"
.text.unlikely

ecomm:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ECOMM(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ECOMM
	ldrsw	x0,[x1,#:lo12:ECOMM]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	ecomm,globl,hidden
