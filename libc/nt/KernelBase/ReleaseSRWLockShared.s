.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_ReleaseSRWLockShared,ReleaseSRWLockShared,1342

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
