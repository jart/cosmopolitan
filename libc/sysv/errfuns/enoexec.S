#include "libc/macros.internal.h"
.text.unlikely

enoexec:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ENOEXEC(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENOEXEC
	ldrsw	x0,[x1,#:lo12:ENOEXEC]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enoexec,globl,hidden
