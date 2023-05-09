#include "libc/macros.internal.h"
.text.unlikely

enopkg:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ENOPKG(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENOPKG
	ldr	w0,[x1,#:lo12:ENOPKG]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enopkg,globl,hidden
