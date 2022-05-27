#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

econnaborted:
	.leafprologue
	mov	ECONNABORTED(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	econnaborted,globl,hidden
