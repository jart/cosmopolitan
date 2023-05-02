#include "libc/nt/codegen.h"
.imp	kernel32,__imp_SetEnvironmentVariableW,SetEnvironmentVariableW,0

#ifdef __x86_64__
	.text.windows
SetEnvironmentVariable:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetEnvironmentVariableW(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetEnvironmentVariable,globl
	.previous
#endif
