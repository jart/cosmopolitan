#include "libc/nt/codegen.h"
.imp	iphlpapi,__imp_GetNumberOfInterfaces,GetNumberOfInterfaces,0

#ifdef __x86_64__
	.text.windows
GetNumberOfInterfaces:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetNumberOfInterfaces(%rip)
	leave
	ret
	.endfn	GetNumberOfInterfaces,globl
	.previous
#endif
