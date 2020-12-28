.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_TryAcquireSRWLockExclusive,TryAcquireSRWLockExclusive,0

	.text.windows
TryAcquireSRWLockExclusive:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_TryAcquireSRWLockExclusive(%rip)
	leave
	ret
	.endfn	TryAcquireSRWLockExclusive,globl
	.previous
