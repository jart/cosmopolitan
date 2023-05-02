#include "libc/nt/codegen.h"
.imp	kernel32,__imp_ReleaseSRWLockShared,ReleaseSRWLockShared,0

#ifdef __x86_64__
	.text.windows
ReleaseSRWLockShared:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_ReleaseSRWLockShared(%rip)
	leave
	ret
	.endfn	ReleaseSRWLockShared,globl
	.previous
#endif
