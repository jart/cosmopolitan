#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetCurrentProcess,GetCurrentProcess,0

#ifdef __x86_64__
	.text.windows
GetCurrentProcess:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_GetCurrentProcess(%rip)
	leave
	ret
	.endfn	GetCurrentProcess,globl
	.previous
#endif
