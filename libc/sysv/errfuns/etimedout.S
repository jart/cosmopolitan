#include "libc/macros.internal.h"
.text.unlikely

etimedout:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ETIMEDOUT(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ETIMEDOUT
	ldrsw	x0,[x1,#:lo12:ETIMEDOUT]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	etimedout,globl,hidden
