#include "libc/macros.internal.h"
.text.unlikely

eafnosupport:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EAFNOSUPPORT(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EAFNOSUPPORT
	ldr	w0,[x1,#:lo12:EAFNOSUPPORT]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eafnosupport,globl,hidden
