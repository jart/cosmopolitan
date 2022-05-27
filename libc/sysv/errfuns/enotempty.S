#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

enotempty:
	.leafprologue
	mov	ENOTEMPTY(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enotempty,globl,hidden
