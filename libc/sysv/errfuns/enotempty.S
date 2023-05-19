#include "libc/macros.internal.h"
.text.unlikely

enotempty:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ENOTEMPTY(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENOTEMPTY
	ldrsw	x0,[x1,#:lo12:ENOTEMPTY]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enotempty,globl,hidden
