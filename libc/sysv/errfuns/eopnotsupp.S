#include "libc/macros.internal.h"
.text.unlikely

eopnotsupp:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EOPNOTSUPP(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EOPNOTSUPP
	ldrsw	x0,[x1,#:lo12:EOPNOTSUPP]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eopnotsupp,globl,hidden
