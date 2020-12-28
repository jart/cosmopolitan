.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetSystemTime,GetSystemTime,0

	.text.windows
GetSystemTime:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetSystemTime(%rip)
	leave
	ret
	.endfn	GetSystemTime,globl
	.previous
