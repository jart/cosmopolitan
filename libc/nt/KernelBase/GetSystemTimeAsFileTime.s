.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetSystemTimeAsFileTime,GetSystemTimeAsFileTime,750

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
