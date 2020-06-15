.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetStartupInfoA,GetStartupInfoA,723

	.text.windows
GetStartupInfoA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetStartupInfoA(%rip)
	leave
	ret
	.endfn	GetStartupInfoA,globl
	.previous
