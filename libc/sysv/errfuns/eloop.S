#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

eloop:	.leafprologue
	mov	ELOOP(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eloop,globl,hidden
