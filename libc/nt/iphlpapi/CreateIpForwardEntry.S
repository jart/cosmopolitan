#include "libc/nt/codegen.h"
.imp	iphlpapi,__imp_CreateIpForwardEntry,CreateIpForwardEntry,0

#ifdef __x86_64__
	.text.windows
CreateIpForwardEntry:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_CreateIpForwardEntry(%rip)
	leave
	ret
	.endfn	CreateIpForwardEntry,globl
	.previous
#endif
