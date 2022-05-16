#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

elibacc:
	.leafprologue
	mov	ELIBACC(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	elibacc,globl,hidden
