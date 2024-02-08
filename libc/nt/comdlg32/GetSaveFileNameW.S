#include "libc/nt/codegen.h"
.imp	comdlg32,__imp_GetSaveFileNameW,GetSaveFileNameW

	.text.windows
	.ftrace1
GetSaveFileName:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetSaveFileNameW(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	GetSaveFileName,globl
	.previous
