#include "libc/macros.internal.h"
.text.unlikely

enotconn:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ENOTCONN(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENOTCONN
	ldrsw	x0,[x1,#:lo12:ENOTCONN]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enotconn,globl,hidden
