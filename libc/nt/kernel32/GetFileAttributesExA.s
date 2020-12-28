.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetFileAttributesExA,GetFileAttributesExA,0

	.text.windows
GetFileAttributesExA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetFileAttributesExA(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetFileAttributesExA,globl
	.previous
