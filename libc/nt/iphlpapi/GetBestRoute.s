.include "o/libc/nt/codegen.inc"
.imp	iphlpapi,__imp_GetBestRoute,GetBestRoute,0

	.text.windows
GetBestRoute:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetBestRoute(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetBestRoute,globl
	.previous
