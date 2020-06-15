.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetSystemTime,GetSystemTime,747

	.text.windows
GetSystemTime:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetSystemTime(%rip)
	leave
	ret
	.endfn	GetSystemTime,globl
	.previous
