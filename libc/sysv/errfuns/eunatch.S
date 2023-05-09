#include "libc/macros.internal.h"
.text.unlikely

eunatch:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EUNATCH(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EUNATCH
	ldr	w0,[x1,#:lo12:EUNATCH]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eunatch,globl,hidden
