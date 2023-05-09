#include "libc/macros.internal.h"
.text.unlikely

enetreset:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ENETRESET(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENETRESET
	ldr	w0,[x1,#:lo12:ENETRESET]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enetreset,globl,hidden
