#include "libc/nt/codegen.h"
.imp	iphlpapi,__imp_FlushIpNetTable,FlushIpNetTable,0

#ifdef __x86_64__
	.text.windows
FlushIpNetTable:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_FlushIpNetTable(%rip)
	leave
	ret
	.endfn	FlushIpNetTable,globl
	.previous
#endif
