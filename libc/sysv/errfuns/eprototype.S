#include "libc/macros.internal.h"
.text.unlikely

eprototype:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EPROTOTYPE(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EPROTOTYPE
	ldr	w0,[x1,#:lo12:EPROTOTYPE]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eprototype,globl,hidden
