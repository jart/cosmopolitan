#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetConsoleOutputCP,GetConsoleOutputCP,0

#ifdef __x86_64__
	.text.windows
GetConsoleOutputCP:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_GetConsoleOutputCP(%rip)
	leave
	ret
	.endfn	GetConsoleOutputCP,globl
	.previous
#endif
