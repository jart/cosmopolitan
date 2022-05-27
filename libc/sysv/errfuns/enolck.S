#include "libc/macros.internal.h"
.text.unlikely

enolck:	.leafprologue
	.profilable
	mov	ENOLCK(%rip),%ecx
	jmp	__errfun
	.endfn	enolck,globl,hidden
