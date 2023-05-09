#include "libc/macros.internal.h"
.text.unlikely

enfile:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ENFILE(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENFILE
	ldr	w0,[x1,#:lo12:ENFILE]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enfile,globl,hidden
