#include "libc/macros.internal.h"
.text.unlikely

exfull:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EXFULL(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EXFULL
	ldrsw	x0,[x1,#:lo12:EXFULL]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	exfull,globl,hidden
