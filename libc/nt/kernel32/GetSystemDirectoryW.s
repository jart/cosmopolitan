.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetSystemDirectoryW,GetSystemDirectoryW,0

	.text.windows
GetSystemDirectory:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetSystemDirectoryW(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetSystemDirectory,globl
	.previous
