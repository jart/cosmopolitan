#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
ekeyexpired:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EKEYEXPIRED(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EKEYEXPIRED
	ldrsw	x0,[x1,#:lo12:EKEYEXPIRED]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	ekeyexpired,globl,hidden
