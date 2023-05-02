#include "libc/nt/codegen.h"
.imp	kernel32,__imp_LockResource,LockResource,0

#ifdef __x86_64__
	.text.windows
LockResource:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_LockResource(%rip)
	leave
	ret
	.endfn	LockResource,globl
	.previous
#endif
