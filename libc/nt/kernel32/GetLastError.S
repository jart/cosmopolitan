#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetLastError,GetLastError,0

#ifdef __x86_64__
	.text.windows
GetLastError:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_GetLastError(%rip)
	leave
	ret
	.endfn	GetLastError,globl
	.previous
#endif
