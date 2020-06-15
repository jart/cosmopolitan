#include "libc/macros.h"
.text.unlikely

enotempty:
	.leafprologue
	.profilable
	mov	ENOTEMPTY(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enotempty,globl,hidden
