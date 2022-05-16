#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

eprototype:
	.leafprologue
	mov	EPROTOTYPE(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eprototype,globl,hidden
