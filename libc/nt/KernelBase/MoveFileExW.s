.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_MoveFileExW,MoveFileExW,1009

	.text.windows
MoveFileEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_MoveFileExW(%rip),%rax
	jmp	__sysv2nt
	.endfn	MoveFileEx,globl
	.previous
