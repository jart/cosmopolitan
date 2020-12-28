.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetProcessHandleCount,GetProcessHandleCount,0

	.text.windows
GetProcessHandleCount:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetProcessHandleCount(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetProcessHandleCount,globl
	.previous
