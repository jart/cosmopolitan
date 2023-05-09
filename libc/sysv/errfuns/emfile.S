#include "libc/macros.internal.h"
.text.unlikely

emfile:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EMFILE(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EMFILE
	ldr	w0,[x1,#:lo12:EMFILE]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	emfile,globl,hidden
