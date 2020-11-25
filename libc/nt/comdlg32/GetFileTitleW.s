.include "o/libc/nt/codegen.inc"
.imp	comdlg32,__imp_GetFileTitleW,GetFileTitleW,112

	.text.windows
GetFileTitle:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetFileTitleW(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetFileTitle,globl
	.previous
