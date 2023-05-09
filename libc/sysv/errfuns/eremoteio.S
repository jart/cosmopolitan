#include "libc/macros.internal.h"
.text.unlikely

eremoteio:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EREMOTEIO(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EREMOTEIO
	ldr	w0,[x1,#:lo12:EREMOTEIO]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eremoteio,globl,hidden
