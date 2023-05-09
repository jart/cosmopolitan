#include "libc/macros.internal.h"
.text.unlikely

eintr:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EINTR(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EINTR
	ldr	w0,[x1,#:lo12:EINTR]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eintr,globl,hidden
