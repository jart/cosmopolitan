#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

elibscn:
	.leafprologue
	mov	ELIBSCN(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	elibscn,globl,hidden
