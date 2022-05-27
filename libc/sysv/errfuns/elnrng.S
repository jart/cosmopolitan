#include "libc/macros.internal.h"
.text.unlikely

elnrng:	.leafprologue
	.profilable
	mov	ELNRNG(%rip),%ecx
	jmp	__errfun
	.endfn	elnrng,globl,hidden
