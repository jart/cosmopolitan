#include "libc/macros.internal.h"
.text.unlikely

econnaborted:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ECONNABORTED(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ECONNABORTED
	ldr	w0,[x1,#:lo12:ECONNABORTED]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	econnaborted,globl,hidden
