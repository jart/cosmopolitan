#include "libc/nt/codegen.h"
.imp	kernel32,__imp_ReadConsoleW,ReadConsoleW,0

#ifdef __x86_64__
	.text.windows
ReadConsole:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ReadConsoleW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	ReadConsole,globl
	.previous
#endif
