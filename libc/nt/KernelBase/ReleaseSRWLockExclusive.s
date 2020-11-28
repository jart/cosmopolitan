.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_ReleaseSRWLockExclusive,ReleaseSRWLockExclusive,1341

	.text.windows
ReleaseSRWLockExclusive:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_ReleaseSRWLockExclusive(%rip)
	leave
	ret
	.endfn	ReleaseSRWLockExclusive,globl
	.previous
