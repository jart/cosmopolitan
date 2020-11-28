.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_AcquireSRWLockShared,AcquireSRWLockShared,684

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
