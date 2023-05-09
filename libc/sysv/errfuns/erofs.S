#include "libc/macros.internal.h"
.text.unlikely

erofs:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EROFS(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EROFS
	ldr	w0,[x1,#:lo12:EROFS]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	erofs,globl,hidden
