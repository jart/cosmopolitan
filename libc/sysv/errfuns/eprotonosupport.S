#include "libc/macros.internal.h"
.text.unlikely

eprotonosupport:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EPROTONOSUPPORT(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EPROTONOSUPPORT
	ldr	w0,[x1,#:lo12:EPROTONOSUPPORT]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eprotonosupport,globl,hidden
