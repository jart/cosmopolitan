.include "o/libc/nt/codegen.inc"
.imp	iphlpapi,__imp_AddIPAddress,AddIPAddress,0

	.text.windows
AddIPAddress:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_AddIPAddress(%rip),%rax
	jmp	__sysv2nt6
	.endfn	AddIPAddress,globl
	.previous
