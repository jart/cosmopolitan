#include "libc/macros.internal.h"
.text.unlikely

el3rst:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EL3RST(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EL3RST
	ldr	w0,[x1,#:lo12:EL3RST]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	el3rst,globl,hidden
