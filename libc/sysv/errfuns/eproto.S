#include "libc/macros.internal.h"
.text.unlikely

eproto:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EPROTO(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EPROTO
	ldr	w0,[x1,#:lo12:EPROTO]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eproto,globl,hidden
