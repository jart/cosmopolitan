#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

enosys:	.leafprologue
	mov	ENOSYS(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enosys,globl,hidden
