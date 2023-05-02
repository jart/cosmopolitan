#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetLogicalDrives,GetLogicalDrives,0

#ifdef __x86_64__
	.text.windows
GetLogicalDrives:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_GetLogicalDrives(%rip)
	leave
	ret
	.endfn	GetLogicalDrives,globl
	.previous
#endif
