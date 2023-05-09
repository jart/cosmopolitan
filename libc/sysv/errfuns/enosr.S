#include "libc/macros.internal.h"
.text.unlikely

enosr:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ENOSR(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENOSR
	ldr	w0,[x1,#:lo12:ENOSR]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enosr,globl,hidden
