#include "libc/macros.internal.h"
.text.unlikely

edeadlk:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EDEADLK(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EDEADLK
	ldr	w0,[x1,#:lo12:EDEADLK]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	edeadlk,globl,hidden
