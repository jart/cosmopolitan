#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetWindowsDirectoryA,GetWindowsDirectoryA

	.text.windows
	.ftrace1
GetWindowsDirectoryA:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_GetWindowsDirectoryA(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetWindowsDirectoryA,globl
	.previous
