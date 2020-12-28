.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetCurrentDirectoryW,GetCurrentDirectoryW,0

	.text.windows
GetCurrentDirectory:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetCurrentDirectoryW(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetCurrentDirectory,globl
	.previous
