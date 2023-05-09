#include "libc/macros.internal.h"
.text.unlikely

enospc:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ENOSPC(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENOSPC
	ldr	w0,[x1,#:lo12:ENOSPC]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enospc,globl,hidden
