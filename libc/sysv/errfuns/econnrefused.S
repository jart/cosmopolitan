#include "libc/macros.internal.h"
.text.unlikely

econnrefused:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ECONNREFUSED(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ECONNREFUSED
	ldr	w0,[x1,#:lo12:ECONNREFUSED]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	econnrefused,globl,hidden
