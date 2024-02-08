#include "libc/nt/codegen.h"
.imp	comdlg32,__imp_ChooseColorW,ChooseColorW

	.text.windows
	.ftrace1
ChooseColor:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_ChooseColorW(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	ChooseColor,globl
	.previous
