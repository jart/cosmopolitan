#include "libc/nt/codegen.h"
.imp	kernel32,__imp_WriteConsoleW,WriteConsoleW,0

#ifdef __x86_64__
	.text.windows
WriteConsole:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WriteConsoleW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	WriteConsole,globl
	.previous
#endif
