.include "o/libc/nt/codegen.inc"
.imp	iphlpapi,__imp_GetBestInterfaceEx,GetBestInterfaceEx,0

	.text.windows
GetBestInterfaceEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetBestInterfaceEx(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetBestInterfaceEx,globl
	.previous
