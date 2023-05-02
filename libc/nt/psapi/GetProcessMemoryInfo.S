#include "libc/nt/codegen.h"
.imp	psapi,__imp_GetProcessMemoryInfo,GetProcessMemoryInfo,0

#ifdef __x86_64__
	.text.windows
GetProcessMemoryInfo:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetProcessMemoryInfo(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetProcessMemoryInfo,globl
	.previous
#endif
