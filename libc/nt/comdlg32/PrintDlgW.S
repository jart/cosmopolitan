#include "libc/nt/codegen.h"
.imp	comdlg32,__imp_PrintDlgW,PrintDlgW

	.text.windows
	.ftrace1
PrintDlg:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_PrintDlgW(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	PrintDlg,globl
	.previous
