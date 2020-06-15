.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetProcessId,GetProcessId,674

	.text.windows
GetProcessId:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetProcessId(%rip)
	leave
	ret
	.endfn	GetProcessId,globl
	.previous
