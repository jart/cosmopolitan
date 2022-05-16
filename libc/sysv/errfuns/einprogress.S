#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

einprogress:
	.leafprologue
	mov	EINPROGRESS(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	einprogress,globl,hidden
