#include "libc/macros.internal.h"
.text.unlikely

eloop:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ELOOP(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ELOOP
	ldrsw	x0,[x1,#:lo12:ELOOP]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eloop,globl,hidden
