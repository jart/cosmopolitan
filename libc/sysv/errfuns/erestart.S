#include "libc/macros.internal.h"
.text.unlikely

erestart:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ERESTART(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ERESTART
	ldr	w0,[x1,#:lo12:ERESTART]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	erestart,globl,hidden
