#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetSystemInfo,GetSystemInfo

	.text.windows
	.ftrace1
GetSystemInfo:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetSystemInfo(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	GetSystemInfo,globl
	.previous
