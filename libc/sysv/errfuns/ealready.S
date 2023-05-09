#include "libc/macros.internal.h"
.text.unlikely

ealready:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EALREADY(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EALREADY
	ldr	w0,[x1,#:lo12:EALREADY]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	ealready,globl,hidden
