#include "libc/macros.internal.h"
.text.unlikely

enocsi:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ENOCSI(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENOCSI
	ldr	w0,[x1,#:lo12:ENOCSI]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enocsi,globl,hidden
