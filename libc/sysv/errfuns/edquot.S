#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

edquot:	.leafprologue
	mov	EDQUOT(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	edquot,globl,hidden
