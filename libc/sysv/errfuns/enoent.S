#include "libc/macros.internal.h"
.text.unlikely

enoent:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ENOENT(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENOENT
	ldr	w0,[x1,#:lo12:ENOENT]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enoent,globl,hidden
