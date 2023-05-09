#include "libc/macros.internal.h"
.text.unlikely

ebadfd:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EBADFD(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EBADFD
	ldr	w0,[x1,#:lo12:EBADFD]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	ebadfd,globl,hidden
