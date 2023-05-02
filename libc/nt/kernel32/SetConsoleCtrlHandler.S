#include "libc/nt/codegen.h"
.imp	kernel32,__imp_SetConsoleCtrlHandler,SetConsoleCtrlHandler,0

#ifdef __x86_64__
	.text.windows
SetConsoleCtrlHandler:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetConsoleCtrlHandler(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetConsoleCtrlHandler,globl
	.previous
#endif
