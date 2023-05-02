#include "libc/nt/codegen.h"
.imp	kernel32,__imp_SetConsoleCP,SetConsoleCP,0

#ifdef __x86_64__
	.text.windows
SetConsoleCP:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_SetConsoleCP(%rip)
	leave
	ret
	.endfn	SetConsoleCP,globl
	.previous
#endif
