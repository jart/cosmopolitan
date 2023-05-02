#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetStdHandle,GetStdHandle,0

#ifdef __x86_64__
	.text.windows
GetStdHandle:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetStdHandle(%rip)
	leave
	ret
	.endfn	GetStdHandle,globl
	.previous
#endif
