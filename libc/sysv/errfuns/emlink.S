#include "libc/macros.internal.h"
.text.unlikely

emlink:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EMLINK(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EMLINK
	ldrsw	x0,[x1,#:lo12:EMLINK]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	emlink,globl,hidden
