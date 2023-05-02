#include "libc/nt/codegen.h"
.imp	kernel32,__imp_FillConsoleOutputAttribute,FillConsoleOutputAttribute,0

#ifdef __x86_64__
	.text.windows
FillConsoleOutputAttribute:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FillConsoleOutputAttribute(%rip),%rax
	jmp	__sysv2nt6
	.endfn	FillConsoleOutputAttribute,globl
	.previous
#endif
