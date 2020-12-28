.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetEnvironmentStringsA,GetEnvironmentStringsA,0

	.text.windows
GetEnvironmentStringsA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetEnvironmentStringsA(%rip)
	leave
	ret
	.endfn	GetEnvironmentStringsA,globl
	.previous
