#include "libc/nt/codegen.h"
.imp	kernel32,__imp_AcquireSRWLockShared,AcquireSRWLockShared,0

#ifdef __x86_64__
	.text.windows
AcquireSRWLockShared:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_AcquireSRWLockShared(%rip)
	leave
	ret
	.endfn	AcquireSRWLockShared,globl
	.previous
#endif
