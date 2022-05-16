#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

enodev:	.leafprologue
	mov	ENODEV(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enodev,globl,hidden
