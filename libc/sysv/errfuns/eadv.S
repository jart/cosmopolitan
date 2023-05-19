#include "libc/macros.internal.h"
.text.unlikely

eadv:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EADV(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EADV
	ldrsw	x0,[x1,#:lo12:EADV]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eadv,globl,hidden
