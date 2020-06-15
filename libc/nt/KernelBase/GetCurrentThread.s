.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetCurrentThread,GetCurrentThread,505

	.text.windows
GetCurrentThread:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_GetCurrentThread(%rip)
	leave
	ret
	.endfn	GetCurrentThread,globl
	.previous
