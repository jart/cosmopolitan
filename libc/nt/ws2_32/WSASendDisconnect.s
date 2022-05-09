.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSASendDisconnect,WSASendDisconnect,97

	.text.windows
WSASendDisconnect:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSASendDisconnect(%rip),%rax
	jmp	__sysv2nt
	.endfn	WSASendDisconnect,globl
	.previous
