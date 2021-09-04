.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GlobalFree,GlobalFree,0

	.text.windows
GlobalFree:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GlobalFree(%rip)
	leave
	ret
	.endfn	GlobalFree,globl
	.previous
