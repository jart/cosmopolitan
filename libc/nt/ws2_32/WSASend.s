.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSASend,WSASend,96

	.text.windows
WSASend:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSASend(%rip),%rax
	jmp	__sysv2nt8
	.endfn	WSASend,globl
	.previous
