#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
eilseq:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
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
