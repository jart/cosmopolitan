.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetCurrentDirectoryA,GetCurrentDirectoryA,0

	.text.windows
GetCurrentDirectoryA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetCurrentDirectoryA(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetCurrentDirectoryA,globl
	.previous
