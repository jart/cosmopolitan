#include "libc/macros.internal.h"
.text.unlikely

ehwpoison:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EHWPOISON(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EHWPOISON
	ldr	w0,[x1,#:lo12:EHWPOISON]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	ehwpoison,globl,hidden
