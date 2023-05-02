#include "libc/nt/codegen.h"
.imp	kernel32,__imp_OfferVirtualMemory,OfferVirtualMemory,0

#ifdef __x86_64__
	.text.windows
OfferVirtualMemory:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_OfferVirtualMemory(%rip),%rax
	jmp	__sysv2nt
	.endfn	OfferVirtualMemory,globl
	.previous
#endif
