.include "o/libc/nt/codegen.inc"
.imp	comdlg32,__imp_GetFileTitleA,GetFileTitleA,111

	.text.windows
GetFileTitleA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetFileTitleA(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetFileTitleA,globl
	.previous
