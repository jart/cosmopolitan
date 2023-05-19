#include "libc/macros.internal.h"
.text.unlikely

emediumtype:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EMEDIUMTYPE(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EMEDIUMTYPE
	ldrsw	x0,[x1,#:lo12:EMEDIUMTYPE]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	emediumtype,globl,hidden
