#include "libc/macros.internal.h"
.text.unlikely

erfkill:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ERFKILL(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ERFKILL
	ldrsw	x0,[x1,#:lo12:ERFKILL]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	erfkill,globl,hidden
