#include "libc/macros.internal.h"
.text.unlikely

enomsg:	.leafprologue
	.profilable
	mov	ENOMSG(%rip),%ecx
	jmp	__errfun
	.endfn	enomsg,globl,hidden
