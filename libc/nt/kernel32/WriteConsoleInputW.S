#include "libc/nt/codegen.h"
.imp	kernel32,__imp_WriteConsoleInputW,WriteConsoleInputW,0

#ifdef __x86_64__
	.text.windows
WriteConsoleInput:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WriteConsoleInputW(%rip),%rax
	jmp	__sysv2nt
	.endfn	WriteConsoleInput,globl
	.previous
#endif
