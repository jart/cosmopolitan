#include "libc/macros.internal.h"
.text.unlikely

enodev:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ENODEV(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENODEV
	ldr	w0,[x1,#:lo12:ENODEV]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enodev,globl,hidden
