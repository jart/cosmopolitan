#include "libc/macros.h"
.text.unlikely

enosys:	.leafprologue
	.profilable
	mov	ENOSYS(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enosys,globl,hidden
