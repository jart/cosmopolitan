#include "libc/nt/codegen.h"
.imp	psapi,__imp_GetProcessMemoryInfo,GetProcessMemoryInfo,0

	.text.windows
GetProcessMemoryInfo:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetProcessMemoryInfo(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetProcessMemoryInfo,globl
	.previous
