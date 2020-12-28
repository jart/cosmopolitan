.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_OfferVirtualMemory,OfferVirtualMemory,0

	.text.windows
OfferVirtualMemory:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_OfferVirtualMemory(%rip),%rax
	jmp	__sysv2nt
	.endfn	OfferVirtualMemory,globl
	.previous
