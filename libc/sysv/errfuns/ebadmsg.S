#include "libc/macros.internal.h"
.text.unlikely

ebadmsg:
	.leafprologue
	.profilable
	mov	EBADMSG(%rip),%ecx
	jmp	__errfun
	.endfn	ebadmsg,globl,hidden
