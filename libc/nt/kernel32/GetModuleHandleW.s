.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetModuleHandleW,GetModuleHandleW,0

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
