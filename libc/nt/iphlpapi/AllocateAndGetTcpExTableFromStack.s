.include "o/libc/nt/codegen.inc"
.imp	iphlpapi,__imp_AllocateAndGetTcpExTableFromStack,AllocateAndGetTcpExTableFromStack,0

	.text.windows
AllocateAndGetTcpExTableFromStack:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_AllocateAndGetTcpExTableFromStack(%rip),%rax
	jmp	__sysv2nt6
	.endfn	AllocateAndGetTcpExTableFromStack,globl
	.previous
