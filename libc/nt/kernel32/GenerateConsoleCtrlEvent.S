#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GenerateConsoleCtrlEvent,GenerateConsoleCtrlEvent,0

#ifdef __x86_64__
	.text.windows
__GenerateConsoleCtrlEvent:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GenerateConsoleCtrlEvent(%rip),%rax
	jmp	__sysv2nt
	.endfn	__GenerateConsoleCtrlEvent,globl
	.previous
#endif
