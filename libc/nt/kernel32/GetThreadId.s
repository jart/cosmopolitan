.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetThreadId,GetThreadId,0

	.text.windows
GetThreadId:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetThreadId(%rip)
	leave
	ret
	.endfn	GetThreadId,globl
	.previous
