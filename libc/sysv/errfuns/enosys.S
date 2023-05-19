#include "libc/macros.internal.h"
.text.unlikely

enosys:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ENOSYS(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENOSYS
	ldrsw	x0,[x1,#:lo12:ENOSYS]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enosys,globl,hidden
