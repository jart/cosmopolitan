#include "libc/macros.internal.h"
.text.unlikely

ebadmsg:
	.leafprologue
	.profilable
	mov	EBADMSG(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	ebadmsg,globl,hidden
