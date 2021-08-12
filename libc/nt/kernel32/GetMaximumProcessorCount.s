.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetMaximumProcessorCount,GetMaximumProcessorCount,627

	.text.windows
GetMaximumProcessorCount:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetMaximumProcessorCount(%rip)
	leave
	ret
	.endfn	GetMaximumProcessorCount,globl
	.previous
