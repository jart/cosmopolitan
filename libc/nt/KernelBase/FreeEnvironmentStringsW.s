.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_FreeEnvironmentStringsW,FreeEnvironmentStringsW,401

	.text.windows
FreeEnvironmentStrings:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_FreeEnvironmentStringsW(%rip)
	leave
	ret
	.endfn	FreeEnvironmentStrings,globl
	.previous
