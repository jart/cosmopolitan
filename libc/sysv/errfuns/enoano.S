#include "libc/macros.internal.h"
.text.unlikely

enoano:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ENOANO(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENOANO
	ldr	w0,[x1,#:lo12:ENOANO]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enoano,globl,hidden
