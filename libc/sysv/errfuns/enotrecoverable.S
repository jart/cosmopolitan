#include "libc/macros.internal.h"
.text.unlikely

enotrecoverable:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ENOTRECOVERABLE(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENOTRECOVERABLE
	ldrsw	x0,[x1,#:lo12:ENOTRECOVERABLE]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enotrecoverable,globl,hidden
