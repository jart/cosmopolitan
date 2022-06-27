.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_GetAddrInfoW,GetAddrInfoW,32

	.text.windows
GetAddrInfo:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetAddrInfoW(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetAddrInfo,globl
	.previous
