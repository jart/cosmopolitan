.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_MoveFileA,MoveFileA,995

	.text.windows
MoveFileA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_MoveFileA(%rip),%rax
	jmp	__sysv2nt
	.endfn	MoveFileA,globl
	.previous
