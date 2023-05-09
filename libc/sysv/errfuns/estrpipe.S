#include "libc/macros.internal.h"
.text.unlikely

estrpipe:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ESTRPIPE(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ESTRPIPE
	ldr	w0,[x1,#:lo12:ESTRPIPE]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	estrpipe,globl,hidden
