#include "libc/macros.internal.h"
.text.unlikely

edestaddrreq:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EDESTADDRREQ(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EDESTADDRREQ
	ldr	w0,[x1,#:lo12:EDESTADDRREQ]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	edestaddrreq,globl,hidden
