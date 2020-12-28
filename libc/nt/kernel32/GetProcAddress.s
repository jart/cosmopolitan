.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetProcAddress,GetProcAddress,0

	.text.windows
GetProcAddress:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetProcAddress(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetProcAddress,globl
	.previous
