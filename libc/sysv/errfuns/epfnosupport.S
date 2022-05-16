#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

epfnosupport:
	.leafprologue
	mov	EPFNOSUPPORT(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	epfnosupport,globl,hidden
