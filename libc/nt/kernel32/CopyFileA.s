.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_CopyFileA,CopyFileA,168

	.text.windows
CopyFileA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CopyFileA(%rip),%rax
	jmp	__sysv2nt
	.endfn	CopyFileA,globl
	.previous
