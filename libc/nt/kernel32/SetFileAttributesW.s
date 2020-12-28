.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_SetFileAttributesW,SetFileAttributesW,0

	.text.windows
SetFileAttributes:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetFileAttributesW(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetFileAttributes,globl
	.previous
