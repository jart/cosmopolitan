.include "o/libc/nt/codegen.inc"
.imp	iphlpapi,__imp_GetAdaptersInfo,GetAdaptersInfo,0

	.text.windows
GetAdaptersInfo:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetAdaptersInfo(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetAdaptersInfo,globl
	.previous
