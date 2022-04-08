.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_CreateDirectoryW,CreateDirectoryW,0

	.text.windows
__CreateDirectory:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateDirectoryW(%rip),%rax
	jmp	__sysv2nt
	.endfn	__CreateDirectory,globl
	.previous
