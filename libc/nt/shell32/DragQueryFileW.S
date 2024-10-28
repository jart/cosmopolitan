#include "libc/nt/codegen.h"
.imp	shell32,__imp_DragQueryFileW,DragQueryFileW

	.text.windows
	.ftrace1
DragQueryFile:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_DragQueryFileW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	DragQueryFile,globl
	.previous
