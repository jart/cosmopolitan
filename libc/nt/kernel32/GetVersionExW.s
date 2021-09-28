.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetVersionExW,GetVersionExW,0

	.text.windows
GetVersionEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetVersionExW(%rip)
	leave
	ret
	.endfn	GetVersionEx,globl
	.previous
