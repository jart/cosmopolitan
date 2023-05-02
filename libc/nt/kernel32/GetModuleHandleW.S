#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetModuleHandleW,GetModuleHandleW,0

#ifdef __x86_64__
	.text.windows
GetModuleHandleW:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetModuleHandleW(%rip)
	leave
	ret
	.endfn	GetModuleHandleW,globl
	.previous
#endif
