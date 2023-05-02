#include "libc/nt/codegen.h"
.imp	kernel32,__imp_SetEvent,SetEvent,0

#ifdef __x86_64__
	.text.windows
SetEvent:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_SetEvent(%rip)
	leave
	ret
	.endfn	SetEvent,globl
	.previous
#endif
