#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetLastError,GetLastError,0

	.text.windows
GetLastError:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_GetLastError(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	GetLastError,globl
	.previous
