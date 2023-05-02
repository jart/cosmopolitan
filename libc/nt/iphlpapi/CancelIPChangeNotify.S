#include "libc/nt/codegen.h"
.imp	iphlpapi,__imp_CancelIPChangeNotify,CancelIPChangeNotify,0

#ifdef __x86_64__
	.text.windows
CancelIPChangeNotify:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_CancelIPChangeNotify(%rip)
	leave
	ret
	.endfn	CancelIPChangeNotify,globl
	.previous
#endif
