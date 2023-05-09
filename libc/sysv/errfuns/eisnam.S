#include "libc/macros.internal.h"
.text.unlikely

eisnam:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EISNAM(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EISNAM
	ldr	w0,[x1,#:lo12:EISNAM]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eisnam,globl,hidden
