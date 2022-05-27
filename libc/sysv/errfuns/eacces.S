#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

eacces:	.leafprologue
	mov	EACCES(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eacces,globl,hidden
