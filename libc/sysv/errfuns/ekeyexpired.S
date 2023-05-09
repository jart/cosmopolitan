#include "libc/macros.internal.h"
.text.unlikely

ekeyexpired:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EKEYEXPIRED(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EKEYEXPIRED
	ldr	w0,[x1,#:lo12:EKEYEXPIRED]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	ekeyexpired,globl,hidden
