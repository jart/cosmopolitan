.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetSystemTimePreciseAsFileTime,GetSystemTimePreciseAsFileTime,751

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
