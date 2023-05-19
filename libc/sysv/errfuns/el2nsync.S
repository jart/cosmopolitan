#include "libc/macros.internal.h"
.text.unlikely

el2nsync:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EL2NSYNC(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EL2NSYNC
	ldrsw	x0,[x1,#:lo12:EL2NSYNC]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	el2nsync,globl,hidden
