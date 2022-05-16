#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

enomsg:	.leafprologue
	mov	ENOMSG(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enomsg,globl,hidden
