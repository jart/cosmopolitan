#include "libc/macros.internal.h"
.text.unlikely

enetunreach:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ENETUNREACH(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENETUNREACH
	ldr	w0,[x1,#:lo12:ENETUNREACH]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enetunreach,globl,hidden
