#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

echrng:	.leafprologue
	mov	ECHRNG(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	echrng,globl,hidden
