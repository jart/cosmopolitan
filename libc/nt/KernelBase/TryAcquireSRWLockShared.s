.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_TryAcquireSRWLockShared,TryAcquireSRWLockShared,1468

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
