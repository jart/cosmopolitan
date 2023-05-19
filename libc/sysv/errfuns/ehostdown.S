#include "libc/macros.internal.h"
.text.unlikely

ehostdown:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EHOSTDOWN(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EHOSTDOWN
	ldrsw	x0,[x1,#:lo12:EHOSTDOWN]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	ehostdown,globl,hidden
