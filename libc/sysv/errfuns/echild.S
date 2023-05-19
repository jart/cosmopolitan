#include "libc/macros.internal.h"
.text.unlikely

echild:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ECHILD(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ECHILD
	ldrsw	x0,[x1,#:lo12:ECHILD]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	echild,globl,hidden
