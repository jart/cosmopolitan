#include "libc/macros.h"
.text.unlikely

enomsg:	.leafprologue
	.profilable
	mov	ENOMSG(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enomsg,globl,hidden
