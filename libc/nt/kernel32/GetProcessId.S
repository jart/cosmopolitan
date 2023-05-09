#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetProcessId,GetProcessId,0

	.text.windows
GetProcessId:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetProcessId(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	GetProcessId,globl
	.previous
