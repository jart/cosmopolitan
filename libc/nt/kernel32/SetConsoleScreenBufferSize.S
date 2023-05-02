#include "libc/nt/codegen.h"
.imp	kernel32,__imp_SetConsoleScreenBufferSize,SetConsoleScreenBufferSize,0

#ifdef __x86_64__
	.text.windows
SetConsoleScreenBufferSize:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetConsoleScreenBufferSize(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetConsoleScreenBufferSize,globl
	.previous
#endif
