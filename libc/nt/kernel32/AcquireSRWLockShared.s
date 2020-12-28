.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_AcquireSRWLockShared,AcquireSRWLockShared,0

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
