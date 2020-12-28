.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_ReleaseSRWLockExclusive,ReleaseSRWLockExclusive,0

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
