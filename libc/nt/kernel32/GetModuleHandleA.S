#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetModuleHandleA,GetModuleHandleA,0

#ifdef __x86_64__
	.text.windows
GetModuleHandle:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetModuleHandleA(%rip)
	leave
	ret
	.endfn	GetModuleHandle,globl
	.previous
#endif
