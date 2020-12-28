.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetCurrentThreadId,GetCurrentThreadId,0

	.text.windows
GetCurrentThreadId:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_GetCurrentThreadId(%rip)
	leave
	ret
	.endfn	GetCurrentThreadId,globl
	.previous
