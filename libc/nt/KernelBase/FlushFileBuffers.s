.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_FlushFileBuffers,FlushFileBuffers,389

	.text.windows
FlushFileBuffers:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_FlushFileBuffers(%rip)
	leave
	ret
	.endfn	FlushFileBuffers,globl
	.previous
