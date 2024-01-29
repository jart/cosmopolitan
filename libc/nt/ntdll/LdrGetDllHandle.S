#include "libc/nt/ntdllimport.h"
.ntimp	LdrGetDllHandle,LdrGetDllHandle

	.text.windows
	.ftrace1
LdrGetDllHandle:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_LdrGetDllHandle(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	LdrGetDllHandle,globl
	.previous
