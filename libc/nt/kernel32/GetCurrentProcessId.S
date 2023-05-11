#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetCurrentProcessId,GetCurrentProcessId,0

	.text.windows
GetCurrentProcessId:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_GetCurrentProcessId(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	GetCurrentProcessId,globl
	.previous