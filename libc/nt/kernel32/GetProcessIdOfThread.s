.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetProcessIdOfThread,GetProcessIdOfThread,0

	.text.windows
GetProcessIdOfThread:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetProcessIdOfThread(%rip)
	leave
	ret
	.endfn	GetProcessIdOfThread,globl
	.previous
