.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetEnvironmentStringsW,GetEnvironmentStringsW,531

	.text.windows
GetEnvironmentStrings:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetEnvironmentStringsW(%rip)
	leave
	ret
	.endfn	GetEnvironmentStrings,globl
	.previous
