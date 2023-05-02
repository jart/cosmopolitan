#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetConsoleScreenBufferInfoEx,GetConsoleScreenBufferInfoEx,0

#ifdef __x86_64__
	.text.windows
GetConsoleScreenBufferInfoEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetConsoleScreenBufferInfoEx(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetConsoleScreenBufferInfoEx,globl
	.previous
#endif
