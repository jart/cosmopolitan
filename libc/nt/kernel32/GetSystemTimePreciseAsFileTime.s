.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetSystemTimePreciseAsFileTime,GetSystemTimePreciseAsFileTime,0

	.text.windows
GetSystemTimePreciseAsFileTime:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetSystemTimePreciseAsFileTime(%rip)
	leave
	ret
	.endfn	GetSystemTimePreciseAsFileTime,globl
	.previous
