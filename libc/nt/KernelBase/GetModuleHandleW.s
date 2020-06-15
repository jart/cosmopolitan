.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetModuleHandleW,GetModuleHandleW,606

	.text.windows
GetModuleHandleW:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetModuleHandleW(%rip)
	leave
	ret
	.endfn	GetModuleHandleW,globl
	.previous
