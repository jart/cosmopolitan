.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_SetFileAttributesA,SetFileAttributesA,0

	.text.windows
SetFileAttributesA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetFileAttributesA(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetFileAttributesA,globl
	.previous
