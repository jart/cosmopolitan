#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

esrmnt:	.leafprologue
	mov	ESRMNT(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	esrmnt,globl,hidden
