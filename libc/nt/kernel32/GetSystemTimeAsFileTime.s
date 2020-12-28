.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetSystemTimeAsFileTime,GetSystemTimeAsFileTime,0

	.text.windows
GetSystemTimeAsFileTime:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetSystemTimeAsFileTime(%rip)
	leave
	ret
	.endfn	GetSystemTimeAsFileTime,globl
	.previous
