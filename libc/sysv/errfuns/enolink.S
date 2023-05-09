#include "libc/macros.internal.h"
.text.unlikely

enolink:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ENOLINK(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENOLINK
	ldr	w0,[x1,#:lo12:ENOLINK]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enolink,globl,hidden
