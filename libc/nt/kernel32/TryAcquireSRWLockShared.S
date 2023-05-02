#include "libc/nt/codegen.h"
.imp	kernel32,__imp_TryAcquireSRWLockShared,TryAcquireSRWLockShared,0

#ifdef __x86_64__
	.text.windows
TryAcquireSRWLockShared:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_TryAcquireSRWLockShared(%rip)
	leave
	ret
	.endfn	TryAcquireSRWLockShared,globl
	.previous
#endif
