#include "libc/macros.internal.h"
.text.unlikely

eisdir:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EISDIR(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EISDIR
	ldr	w0,[x1,#:lo12:EISDIR]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eisdir,globl,hidden
