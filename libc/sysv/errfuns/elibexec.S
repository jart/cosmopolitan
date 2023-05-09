#include "libc/macros.internal.h"
.text.unlikely

elibexec:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ELIBEXEC(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ELIBEXEC
	ldr	w0,[x1,#:lo12:ELIBEXEC]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	elibexec,globl,hidden
