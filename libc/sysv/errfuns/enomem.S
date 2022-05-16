#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

enomem:	.leafprologue
	mov	ENOMEM(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enomem,globl,hidden
