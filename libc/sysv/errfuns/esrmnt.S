#include "libc/macros.internal.h"
.text.unlikely

esrmnt:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ESRMNT(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ESRMNT
	ldrsw	x0,[x1,#:lo12:ESRMNT]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	esrmnt,globl,hidden
