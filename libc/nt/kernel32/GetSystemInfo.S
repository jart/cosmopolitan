#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetSystemInfo,GetSystemInfo,0

#ifdef __x86_64__
	.text.windows
GetSystemInfo:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetSystemInfo(%rip)
	leave
	ret
	.endfn	GetSystemInfo,globl
	.previous
#endif
