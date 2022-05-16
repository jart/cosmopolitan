#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

eremoteio:
	.leafprologue
	mov	EREMOTEIO(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eremoteio,globl,hidden
