#include "libc/macros.internal.h"
.text.unlikely

ebusy:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EBUSY(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EBUSY
	ldrsw	x0,[x1,#:lo12:EBUSY]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	ebusy,globl,hidden
