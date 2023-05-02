#include "libc/nt/codegen.h"
.imp	comdlg32,__imp_PrintDlgW,PrintDlgW,123

#ifdef __x86_64__
	.text.windows
PrintDlg:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_PrintDlgW(%rip)
	leave
	ret
	.endfn	PrintDlg,globl
	.previous
#endif
