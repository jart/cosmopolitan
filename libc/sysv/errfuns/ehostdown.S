#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

ehostdown:
	.leafprologue
	mov	EHOSTDOWN(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	ehostdown,globl,hidden
