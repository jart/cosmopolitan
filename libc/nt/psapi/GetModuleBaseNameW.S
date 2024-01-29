#include "libc/nt/codegen.h"
.imp	psapi,__imp_GetModuleBaseNameW,GetModuleBaseNameW

	.text.windows
	.ftrace1
GetModuleBaseName:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_GetModuleBaseNameW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetModuleBaseName,globl
	.previous
