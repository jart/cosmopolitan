#include "libc/nt/codegen.h"
.imp	comdlg32,__imp_GetFileTitleW,GetFileTitleW

	.text.windows
	.ftrace1
GetFileTitle:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_GetFileTitleW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetFileTitle,globl
	.previous
