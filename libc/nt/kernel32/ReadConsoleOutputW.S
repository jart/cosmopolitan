#include "libc/nt/codegen.h"
.imp	kernel32,__imp_ReadConsoleOutputW,ReadConsoleOutputW,0

#ifdef __x86_64__
	.text.windows
ReadConsoleOutput:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ReadConsoleOutputW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	ReadConsoleOutput,globl
	.previous
#endif
