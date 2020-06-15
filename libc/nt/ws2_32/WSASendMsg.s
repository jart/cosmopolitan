.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSASendMsg,WSASendMsg,98

	.text.windows
WSASendMsg:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSASendMsg(%rip),%rax
	jmp	__sysv2nt6
	.endfn	WSASendMsg,globl
	.previous
