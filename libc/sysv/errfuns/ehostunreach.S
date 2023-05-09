#include "libc/macros.internal.h"
.text.unlikely

ehostunreach:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EHOSTUNREACH(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EHOSTUNREACH
	ldr	w0,[x1,#:lo12:EHOSTUNREACH]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	ehostunreach,globl,hidden
