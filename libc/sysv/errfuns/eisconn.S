#include "libc/macros.internal.h"
.text.unlikely

eisconn:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EISCONN(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EISCONN
	ldrsw	x0,[x1,#:lo12:EISCONN]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eisconn,globl,hidden
