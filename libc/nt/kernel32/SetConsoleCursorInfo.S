#include "libc/nt/codegen.h"
.imp	kernel32,__imp_SetConsoleCursorInfo,SetConsoleCursorInfo,0

#ifdef __x86_64__
	.text.windows
SetConsoleCursorInfo:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetConsoleCursorInfo(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetConsoleCursorInfo,globl
	.previous
#endif
