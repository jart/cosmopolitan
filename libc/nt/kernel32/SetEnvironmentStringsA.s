.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_SetEnvironmentStringsA,SetEnvironmentStringsA,1304

	.text.windows
SetEnvironmentStringsA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_SetEnvironmentStringsA(%rip)
	leave
	ret
	.endfn	SetEnvironmentStringsA,globl
	.previous
