#include "libc/macros.internal.h"
.text.unlikely

enotnam:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ENOTNAM(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENOTNAM
	ldrsw	x0,[x1,#:lo12:ENOTNAM]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enotnam,globl,hidden
