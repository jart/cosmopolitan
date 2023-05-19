#include "libc/macros.internal.h"
.text.unlikely

edquot:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EDQUOT(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EDQUOT
	ldrsw	x0,[x1,#:lo12:EDQUOT]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	edquot,globl,hidden
