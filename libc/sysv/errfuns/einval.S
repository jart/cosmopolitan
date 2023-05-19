#include "libc/macros.internal.h"
.text.unlikely

einval:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EINVAL(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EINVAL
	ldrsw	x0,[x1,#:lo12:EINVAL]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	einval,globl,hidden
