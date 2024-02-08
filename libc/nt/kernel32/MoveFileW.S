#include "libc/nt/codegen.h"
.imp	kernel32,__imp_MoveFileW,MoveFileW

	.text.windows
	.ftrace1
MoveFile:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_MoveFileW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	MoveFile,globl
	.previous
