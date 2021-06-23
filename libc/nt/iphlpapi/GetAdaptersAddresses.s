.include "o/libc/nt/codegen.inc"
.imp	iphlpapi,__imp_GetAdaptersAddresses,GetAdaptersAddresses,67

	.text.windows
GetAdaptersAddresses:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetAdaptersAddresses(%rip),%rax
	jmp	__sysv2nt6
	.endfn	GetAdaptersAddresses,globl
	.previous
