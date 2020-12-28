.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_AcquireSRWLockExclusive,AcquireSRWLockExclusive,0

	.text.windows
AcquireSRWLockExclusive:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_AcquireSRWLockExclusive(%rip)
	leave
	ret
	.endfn	AcquireSRWLockExclusive,globl
	.previous
