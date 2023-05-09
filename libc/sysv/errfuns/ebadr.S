#include "libc/macros.internal.h"
.text.unlikely

ebadr:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EBADR(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EBADR
	ldr	w0,[x1,#:lo12:EBADR]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	ebadr,globl,hidden
