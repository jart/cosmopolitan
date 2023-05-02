#include "libc/nt/codegen.h"
.imp	kernel32,__imp_SetConsoleCursorPosition,SetConsoleCursorPosition,0

#ifdef __x86_64__
	.text.windows
SetConsoleCursorPosition:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetConsoleCursorPosition(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetConsoleCursorPosition,globl
	.previous
#endif
