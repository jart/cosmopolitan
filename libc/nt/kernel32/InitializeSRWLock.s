.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_InitializeSRWLock,InitializeSRWLock,0

	.text.windows
InitializeSRWLock:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_InitializeSRWLock(%rip)
	leave
	ret
	.endfn	InitializeSRWLock,globl
	.previous
