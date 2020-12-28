.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_CreateDirectoryA,CreateDirectoryA,0

	.text.windows
CreateDirectoryA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateDirectoryA(%rip),%rax
	jmp	__sysv2nt
	.endfn	CreateDirectoryA,globl
	.previous
