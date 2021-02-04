.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_ioctlsocket,ioctlsocket,10

	.text.windows
__sys_ioctlsocket_nt:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ioctlsocket(%rip),%rax
	jmp	__sysv2nt
	.endfn	__sys_ioctlsocket_nt,globl
	.previous
