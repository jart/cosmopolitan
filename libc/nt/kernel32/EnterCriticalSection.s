.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_EnterCriticalSection,EnterCriticalSection,0

	.text.windows
EnterCriticalSection:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_EnterCriticalSection(%rip)
	leave
	ret
	.endfn	EnterCriticalSection,globl
	.previous
