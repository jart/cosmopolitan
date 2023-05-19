#include "libc/macros.internal.h"
.text.unlikely

eaddrnotavail:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EADDRNOTAVAIL(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EADDRNOTAVAIL
	ldrsw	x0,[x1,#:lo12:EADDRNOTAVAIL]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eaddrnotavail,globl,hidden
