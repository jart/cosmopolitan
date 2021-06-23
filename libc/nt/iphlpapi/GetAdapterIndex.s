.include "o/libc/nt/codegen.inc"
.imp	iphlpapi,__imp_GetAdapterIndex,GetAdapterIndex,0

	.text.windows
GetAdapterIndex:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetAdapterIndex(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetAdapterIndex,globl
	.previous
