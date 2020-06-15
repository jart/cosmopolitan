.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_SetEnvironmentStringsW,SetEnvironmentStringsW,1498

	.text.windows
SetEnvironmentStrings:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_SetEnvironmentStringsW(%rip)
	leave
	ret
	.endfn	SetEnvironmentStrings,globl
	.previous
