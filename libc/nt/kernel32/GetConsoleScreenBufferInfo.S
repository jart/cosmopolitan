#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetConsoleScreenBufferInfo,GetConsoleScreenBufferInfo,0

#ifdef __x86_64__
	.text.windows
GetConsoleScreenBufferInfo:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetConsoleScreenBufferInfo(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetConsoleScreenBufferInfo,globl
	.previous
#endif
