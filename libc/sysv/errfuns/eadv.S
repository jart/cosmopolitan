#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

eadv:	.leafprologue
	mov	EADV(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eadv,globl,hidden
