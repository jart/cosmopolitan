#include "libc/macros.internal.h"
.text.unlikely

eremote:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EREMOTE(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EREMOTE
	ldrsw	x0,[x1,#:lo12:EREMOTE]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eremote,globl,hidden
