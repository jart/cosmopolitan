#include "libc/macros.internal.h"
.text.unlikely

eperm:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EPERM(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EPERM
	ldrsw	x0,[x1,#:lo12:EPERM]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eperm,globl,hidden
