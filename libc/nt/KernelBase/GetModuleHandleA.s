.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetModuleHandleA,GetModuleHandleA,603

	.text.windows
GetModuleHandle:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetModuleHandleA(%rip)
	leave
	ret
	.endfn	GetModuleHandle,globl
	.previous
