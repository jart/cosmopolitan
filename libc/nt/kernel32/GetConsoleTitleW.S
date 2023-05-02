#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetConsoleTitleW,GetConsoleTitleW,0

#ifdef __x86_64__
	.text.windows
GetConsoleTitle:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetConsoleTitleW(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetConsoleTitle,globl
	.previous
#endif
