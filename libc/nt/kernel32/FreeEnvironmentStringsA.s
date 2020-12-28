.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_FreeEnvironmentStringsA,FreeEnvironmentStringsA,0

	.text.windows
FreeEnvironmentStringsA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_FreeEnvironmentStringsA(%rip)
	leave
	ret
	.endfn	FreeEnvironmentStringsA,globl
	.previous
