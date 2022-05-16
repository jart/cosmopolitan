#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

enotty:	.leafprologue
	mov	ENOTTY(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enotty,globl,hidden
