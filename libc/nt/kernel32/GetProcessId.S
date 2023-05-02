#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetProcessId,GetProcessId,0

#ifdef __x86_64__
	.text.windows
GetProcessId:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetProcessId(%rip)
	leave
	ret
	.endfn	GetProcessId,globl
	.previous
#endif
