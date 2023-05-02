#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetModuleHandleExW,GetModuleHandleExW,0

#ifdef __x86_64__
	.text.windows
GetModuleHandleEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetModuleHandleExW(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetModuleHandleEx,globl
	.previous
#endif
