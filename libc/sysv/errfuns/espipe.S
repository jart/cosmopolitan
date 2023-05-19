#include "libc/macros.internal.h"
.text.unlikely

espipe:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ESPIPE(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ESPIPE
	ldrsw	x0,[x1,#:lo12:ESPIPE]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	espipe,globl,hidden
