#include "libc/macros.internal.h"
.text.unlikely

edotdot:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EDOTDOT(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EDOTDOT
	ldr	w0,[x1,#:lo12:EDOTDOT]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	edotdot,globl,hidden
