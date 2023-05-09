#include "libc/macros.internal.h"
.text.unlikely

ebadrqc:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EBADRQC(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EBADRQC
	ldr	w0,[x1,#:lo12:EBADRQC]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	ebadrqc,globl,hidden
