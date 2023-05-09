#include "libc/macros.internal.h"
.text.unlikely

enxio:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ENXIO(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENXIO
	ldr	w0,[x1,#:lo12:ENXIO]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enxio,globl,hidden
