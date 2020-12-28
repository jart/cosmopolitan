.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetFileAttributesExW,GetFileAttributesExW,0

	.text.windows
GetFileAttributesEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetFileAttributesExW(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetFileAttributesEx,globl
	.previous
