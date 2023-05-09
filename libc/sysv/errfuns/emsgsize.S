#include "libc/macros.internal.h"
.text.unlikely

emsgsize:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EMSGSIZE(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EMSGSIZE
	ldr	w0,[x1,#:lo12:EMSGSIZE]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	emsgsize,globl,hidden
