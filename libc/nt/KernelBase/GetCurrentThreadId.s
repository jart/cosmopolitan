.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetCurrentThreadId,GetCurrentThreadId,506

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
