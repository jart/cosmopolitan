#include "libc/macros.internal.h"
.text.unlikely

eilseq:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EILSEQ(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EILSEQ
	ldrsw	x0,[x1,#:lo12:EILSEQ]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eilseq,globl,hidden
