#include "libc/macros.internal.h"
.text.unlikely

exdev:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EXDEV(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EXDEV
	ldr	w0,[x1,#:lo12:EXDEV]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	exdev,globl,hidden
