#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetLogicalDrives,GetLogicalDrives,0

	.text.windows
GetLogicalDrives:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_GetLogicalDrives(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	GetLogicalDrives,globl
	.previous
