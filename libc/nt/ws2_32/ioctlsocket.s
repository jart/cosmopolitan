.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_ioctlsocket,ioctlsocket,10

	.text.windows
__ioctlsocket$nt:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ioctlsocket(%rip),%rax
	jmp	__sysv2nt
	.endfn	__ioctlsocket$nt,globl
	.previous
