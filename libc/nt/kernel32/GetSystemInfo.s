.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetSystemInfo,GetSystemInfo,0

	.text.windows
GetSystemInfo:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetSystemInfo(%rip)
	leave
	ret
	.endfn	GetSystemInfo,globl
	.previous
