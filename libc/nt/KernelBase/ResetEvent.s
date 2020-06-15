.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_ResetEvent,ResetEvent,1410

	.text.windows
ResetEvent:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_ResetEvent(%rip)
	leave
	ret
	.endfn	ResetEvent,globl
	.previous
