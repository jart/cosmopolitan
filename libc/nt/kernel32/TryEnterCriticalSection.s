.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_TryEnterCriticalSection,TryEnterCriticalSection,0

	.text.windows
TryEnterCriticalSection:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_TryEnterCriticalSection(%rip)
	leave
	ret
	.endfn	TryEnterCriticalSection,globl
	.previous
