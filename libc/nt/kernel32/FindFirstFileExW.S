#include "libc/nt/codegen.h"
.imp	kernel32,__imp_FindFirstFileExW,FindFirstFileExW

	.text.windows
	.ftrace1
FindFirstFileEx:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_FindFirstFileExW(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	FindFirstFileEx,globl
	.previous
