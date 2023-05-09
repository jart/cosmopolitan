#include "libc/macros.internal.h"
.text.unlikely

enokey:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ENOKEY(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENOKEY
	ldr	w0,[x1,#:lo12:ENOKEY]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enokey,globl,hidden
