#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetEnvironmentVariableW,GetEnvironmentVariableW,0

#ifdef __x86_64__
	.text.windows
GetEnvironmentVariable:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetEnvironmentVariableW(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetEnvironmentVariable,globl
	.previous
#endif
