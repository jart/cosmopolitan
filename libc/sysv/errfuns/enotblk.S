#include "libc/macros.internal.h"
.text.unlikely

enotblk:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ENOTBLK(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENOTBLK
	ldr	w0,[x1,#:lo12:ENOTBLK]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enotblk,globl,hidden
