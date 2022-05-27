#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

ebadr:	.leafprologue
	mov	EBADR(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	ebadr,globl,hidden
