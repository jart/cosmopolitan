#include "libc/macros.internal.h"
.text.unlikely

enostr:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ENOSTR(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENOSTR
	ldr	w0,[x1,#:lo12:ENOSTR]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enostr,globl,hidden
