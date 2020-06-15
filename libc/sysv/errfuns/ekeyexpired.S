#include "libc/macros.h"
.text.unlikely

ekeyexpired:
	.leafprologue
	.profilable
	mov	EKEYEXPIRED(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	ekeyexpired,globl,hidden
