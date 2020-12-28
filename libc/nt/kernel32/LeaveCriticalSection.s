.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_LeaveCriticalSection,LeaveCriticalSection,0

	.text.windows
LeaveCriticalSection:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_LeaveCriticalSection(%rip)
	leave
	ret
	.endfn	LeaveCriticalSection,globl
	.previous
