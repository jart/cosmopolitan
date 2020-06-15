.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_MoveFileExA,MoveFileExA,996

	.text.windows
MoveFileExA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_MoveFileExA(%rip),%rax
	jmp	__sysv2nt
	.endfn	MoveFileExA,globl
	.previous
