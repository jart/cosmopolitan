.include "o/libc/nt/codegen.inc"
.imp	iphlpapi,__imp_GetBestInterface,GetBestInterface,0

	.text.windows
GetBestInterface:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetBestInterface(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetBestInterface,globl
	.previous
