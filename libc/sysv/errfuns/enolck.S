#include "libc/macros.internal.h"
.text.unlikely

enolck:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ENOLCK(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENOLCK
	ldrsw	x0,[x1,#:lo12:ENOLCK]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enolck,globl,hidden
