.include "o/libc/nt/codegen.inc"
.imp	MsWSock,__imp_AcceptEx,AcceptEx,1

	.text.windows
AcceptEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_AcceptEx(%rip),%rax
	jmp	__sysv2nt8
	.endfn	AcceptEx,globl
	.previous
