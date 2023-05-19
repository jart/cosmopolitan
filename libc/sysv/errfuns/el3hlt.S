#include "libc/macros.internal.h"
.text.unlikely

el3hlt:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EL3HLT(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EL3HLT
	ldrsw	x0,[x1,#:lo12:EL3HLT]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	el3hlt,globl,hidden
