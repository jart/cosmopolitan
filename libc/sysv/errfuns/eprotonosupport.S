#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

eprotonosupport:
	.leafprologue
	mov	EPROTONOSUPPORT(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eprotonosupport,globl,hidden
