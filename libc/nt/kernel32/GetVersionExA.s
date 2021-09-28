.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetVersionExA,GetVersionExA,0

	.text.windows
GetVersionExA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetVersionExA(%rip)
	leave
	ret
	.endfn	GetVersionExA,globl
	.previous
