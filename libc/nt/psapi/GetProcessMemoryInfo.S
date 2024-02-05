#include "libc/nt/codegen.h"
.imp	psapi,__imp_GetProcessMemoryInfo,GetProcessMemoryInfo

	.text.windows
	.ftrace1
GetProcessMemoryInfo:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_GetProcessMemoryInfo(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetProcessMemoryInfo,globl
	.previous
