#include "libc/macros.internal.h"
.text.unlikely

eio:	.leafprologue
	.profilable
	mov	EIO(%rip),%ecx
	jmp	__errfun
	.endfn	eio,globl,hidden
