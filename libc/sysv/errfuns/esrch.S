#include "libc/macros.internal.h"
.text.unlikely

esrch:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ESRCH(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ESRCH
	ldrsw	x0,[x1,#:lo12:ESRCH]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	esrch,globl,hidden
